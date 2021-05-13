#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QBrush>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDebug>
#include <QElapsedTimer>
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

    static int judge_rule(int i,int j);  //四四禁手、五五禁手
    static int robot(int color); //机器，color代表棋子，1是黑，2是白

    static int times;    //已经下的步数
    static int X,Y;   //X,Y是坐标点，左上角为（1,1）
    static int situations[20][20];  //记录棋局
    static int pattern;   //模式设置 1、人机  2、双人  3、电脑
    static int gamestart;  //游戏是否开始
    static int order;  //人机玩家先手或者后手，先手为1，后手为2

private:
    Ui::MainWindow *ui;
    robot_thread *Robot_thread;

signals:
    void back();

public slots:
    void deal_robot_done(int x,int y);
    void stop_thread();
    void deal_robot_final_done();
};
#endif // MAINWINDOW_H
