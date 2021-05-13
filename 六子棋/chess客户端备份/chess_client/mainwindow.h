#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMouseEvent>
#include <QDebug>
#include "tcp_connect.h"
#include "mymsgbox.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *);    //绘图事件
    void mousePressEvent(QMouseEvent *ev);  //鼠标事件
    void reset_all();       //棋局重置
    void disconnect_from_server();      //与服务端断开连接

private:
    Ui::MainWindow *ui;
    tcp_connect *connect_widget = nullptr;
    QTcpSocket *tcpsocket = nullptr;

    int times = 0;
    int order = 0;
    int situations[20][20] = {{0}};
    int X,Y;
    int is_connected = 0;
    mymsgbox *msg = nullptr;
    int whether_reset_when_msg_close = 0;

public slots:
    void connect_to_server();
    void start_connect();
    void read_data_from_server();
    void judge_whether_reset_when_msg_close();
};
#endif // MAINWINDOW_H
