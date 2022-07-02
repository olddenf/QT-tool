#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFileDialog>
#include<QDebug>
#include<QTcpServer>
#include<QTcpSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

void MainWindow::sendData()
{
    timer->stop();
    quint64 len=0;

    do{
        char buf[1024]={0};
        len=0;
        len=send_File->read(buf,sizeof(buf));
        len=tcpSocket->write(buf,len);
//        if(len==1024)
//        {

//        }
    }
    while(len>0);
}

void MainWindow::receive_file()
{
    QByteArray buf=tcpSocket_client->readAll();


    if(isStart)
    {
        isStart=false;
        re_filename=QString(buf).section("##",0,0);
        re_filesize=QString(buf).section("##",1,1).toInt();

        this->savePath=QFileDialog::getSaveFileUrl(this,"选择路径").toString();
        this->reve_file=new QFile();
        reve_file->setFileName(savePath+re_filename);
        if(reve_file->open(QIODevice::WriteOnly|QIODevice::Append))
        {
            ui->textEdit->append(savePath+re_filename);
            ui->textEdit->append("size:"+QString::number(re_filesize));
        }

    }
    else
    {
        quint64 len=reve_file->write(buf);
        if(len>0)
        {
            re_len+=len;
        }
        if(re_len==re_filesize)
        {
            ui->textEdit->append("文件传输完成");
        }
    }
}

void MainWindow::server_client()//
{
    //服务器与客户端连接
    isServerConnectClient=true;
    ui->textEdit->append("客户端已连接，可以传输");
    tcpSocket_client=tcpServer->nextPendingConnection();//建立socket
    //显示客户端的ip
    ui->textEdit->append("IP:");
    ui->textEdit->append(tcpSocket_client->peerAddress().toString());
    ui->textEdit->append("port:");
    ui->textEdit->append(QString::number(tcpSocket_client->peerPort()));
    connect(tcpSocket_client,SIGNAL(disconnected()),this,SLOT(server_dis_client()));
//    connect(tcpSocket_client,SIGNAL(readyRead()),this,SLOT(receive_file));//接收信号
    //修改按钮
    ui->pushButton_host->setText("关闭端口");
}

void MainWindow::server_dis_client()
{

    //服务器与客户端断开
    isServerConnectClient=false;
    ui->textEdit->append("服务器与客户端断开");
    ui->pushButton_host->setText("打开端口");
}

void MainWindow::client_dis_server()
{
    //客户端与服务器断开
    isClientConnectServer=false;
//    tcpSocket->close();//关闭连接
    //文本提示
    ui->textEdit->append("客户端与服务器断开");
    ui->pushButton_connect->setText("连接端口");
}

void MainWindow::client_server()
{
    this->isClientConnectServer=true;
    //客户端与服务器连接
    ui->textEdit->append("客户端与服务器连接成功");
    //修改按钮
    ui->pushButton_connect->setText("断开连接");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_file_clicked()//打开文件按钮
{
    this->filePath=QFileDialog::getOpenFileName(this,"选择文件","/","(*.*)");
    if(this->filePath=="")
    {
        ui->textEdit->append("请选择文件");
    }
    ui->textEdit->append(this->filePath);

    this->send_File=new QFile(filePath);//显示文件路径
    if(!this->send_File->open(QFile::ReadOnly))//打开文件
    {
        ui->textEdit->append("文件打开失败");
    }
    else
    {
        qint64 filesize=this->send_File->size();
        ui->textEdit->append("size:"+QString::number(filesize));//显示文件大小
    }
}

void MainWindow::on_pushButton_host_clicked()
{
    if(this->tcpSocket!=Q_NULLPTR)
    {
        ui->textEdit->append("当前为客户端，不可做服务器！");
        return;
    }//避免服务器与客户端冲突
    if(!this->isServerConnectClient)
    {
        tcpServer=new QTcpServer(this);
        qint64 port=8108;

        connect(tcpServer,SIGNAL(newConnection()),this,SLOT(server_client()));




        if (this->tcpServer->listen(QHostAddress::AnyIPv4,port)) {//监听端口
            //设置提示文本
            ui->textEdit->clear();
            ui->textEdit->append("当前地址为:");
            ui->textEdit->append(tcpServer->serverAddress().toString());
            ui->textEdit->append("当前端口为:");
            ui->textEdit->append(QString::number(port));
        }
        else
        {
            ui->textEdit->setText("服务器开启失败");
        }

    }
    else
    {
        //开启端口后再次点击端口-》关闭端口
        tcpServer->close();//关闭服务器
        delete tcpServer;//释放指针
        tcpServer=Q_NULLPTR;

        //设置按钮内容
        ui->pushButton_host->setText("打开端口");

        //设置文本框提示内容
        ui->textEdit->append("服务器已关闭");

    }
}

void MainWindow::on_pushButton_connect_clicked()
{
    if(this->tcpServer!=Q_NULLPTR)
    {
        ui->textEdit->append("当前为服务器，不可做客户端！");
        return;
    }//避免服务器与客户端冲突
    QString IP;
    QString PORT;
    QString str_ip_port=ui->lineEdit->text();//获取输入框的内容
    if(str_ip_port=="")
    {
        ui->textEdit->append("请输入地址!");
    }
    else
    {
        IP=str_ip_port.mid(0,str_ip_port.length()-5);//筛选获取ip
        PORT=str_ip_port.mid(str_ip_port.length()-4,4);//筛选获取port
        ui->textEdit->append("IP:");
        ui->textEdit->append(IP);
        ui->textEdit->append("port:");
        ui->textEdit->append(PORT);

        //连接服务器
        tcpSocket=new QTcpSocket();
        connect(tcpSocket,SIGNAL(connected()),this,SLOT(client_server()));
        connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(client_dis_server()));
        connect(timer,SIGNAL(timeout()),this,SLOT(sendData()));


        if(this->isClientConnectServer)
        {
            tcpSocket->disconnectFromHost();//断开连接
            tcpSocket->close();
            isClientConnectServer=false;
            delete tcpSocket;
            tcpSocket=Q_NULLPTR;
            qDebug()<<"断开连接";
        }
        else
        {
            tcpSocket->connectToHost(IP,PORT.toUShort(),QIODevice::ReadWrite);//连接服务器
            if(tcpSocket->waitForConnected())
            {
                qDebug()<<"连接到服务器"<<endl;
            }
        }
    }


}

void MainWindow::on_pushButton_transfer_clicked()
{
    QString head=QString("%1##%2").arg(send_File->fileName()).arg(send_File->size());

    qint64 len=tcpSocket->write(head.toUtf8());
    if(len>0)
    {
        timer->start(20);
    }
    else
    {
        ui->textEdit->append("文件发送失败！");
    }
}

