#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QBrush>
#include <QMouseEvent>
#include <QDebug>
#include <QElapsedTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QTimer>
#include <QTime>
#include <QMutex>
#include <ui_mainwindow.h>
#include <mymessagebox.h>
#include <robot_thread.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class robot_thread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *);    //绘图事件
    void mousePressEvent(QMouseEvent *ev);  //鼠标事件
    bool judge_win(int i,int j);   //输赢判断
    void backmenu();  //返回主菜单
    void reset_all();  //重置棋盘
    void set_pattern(int p);  //设置模式
    void is_backed_menu();  //表示已经返回主菜单
    void pattern_five_set_order();      //模式五（客户端对战）设置随机抽取先后手

    static int judge_rule(int i,int j);  //四四禁手、五五禁手
    static int robot(int color); //机器，color代表棋子，1是黑，2是白

    static int times;    //已经下的步数
    static int X,Y;   //X,Y是坐标点，左上角为（1,1）
    static int situations[20][20];  //记录棋局
    static int pattern;   //模式设置 1、人机  2、本地双人  3、电脑   4、服务端与客户端   5、客户端与客户端
    static int gamestart;  //游戏是否开始
    static int order;  //人机玩家先手或者后手，先手为1，后手为2
    static int is_listen;   //是否在监听，1为是，0为否

private:
    Ui::MainWindow *ui;

    robot_thread *Robot_thread;         //AI线程
    QTcpServer *server = NULL;         //服务端套接字
    QTcpSocket *socket = NULL;
     QTcpSocket *temp_socket = NULL;
    QList<QTcpSocket*> tcpsocket_list;          //QTcpSocket容器
    int client_num = 0;                         //目前客户数量
    int is_to_back_menu = 0;              //判断是否将返回主菜单
    int is_to_restart = 0;              //判断是否点击了重新开始
    MymessageBox *msg = nullptr;
    int whether_reset_when_msg_close = 0;

signals:
    void back();

public slots:
    void deal_robot_done(int x,int y);
    void stop_thread();
    void deal_robot_final_done();
    void server_new_connect();
    void socket_read_data();
    void socket_disconnected();
    void start_listen();
    void judge_whether_reset_when_msg_close();
};
#endif // MAINWINDOW_H
