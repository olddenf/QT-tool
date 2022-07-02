#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QFile>
#include<QTcpSocket>
#include<QTcpServer>
#include<QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_file_clicked();

    void on_pushButton_host_clicked();

    void on_pushButton_connect_clicked();

    void server_client();

    void server_dis_client();

    void client_server();

    void client_dis_server();

    void sendData();

    void on_pushButton_transfer_clicked();

    void receive_file();

    void on_pushButton_save_clicked();

private:
    QString filePath;//文件路径
    QFile *send_File;//文件
    QFile *reve_file;//接收的文件
    QString savePath;//接收文件保存的路径
    QString re_filename;
    quint64 re_filesize;
    quint64 re_len=0;

    bool isClientConnectServer=false;//客户端是否连接到客户端
    bool isServerConnectClient=false;//服务器是否连接到客户端
    bool isStart=true;

    QTcpServer *tcpServer=Q_NULLPTR;//服务器
    QTcpSocket *tcpSocket=Q_NULLPTR;//客户端
    QTcpSocket *tcpSocket_client=Q_NULLPTR;//服务器中用于同client交互的套接字

    QTimer *timer;//定时器

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
