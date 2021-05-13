#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QBrush>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDebug>
#include <QElapsedTimer>

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
    bool judge_win(int i,int j);   //输赢判断
    int judge_rule(int i,int j);  //四四禁手、五五禁手
    void backmenu();  //返回主菜单
    void reset_all();  //重置棋盘
    void set_pattern(int p);  //设置模式
    int robot(int color); //机器，color代表棋子，1是黑，2是白

private:
    Ui::MainWindow *ui;
    int times = 0;    //已经下的步数
    int X,Y;   //X,Y是坐标点，左上角为（1,1）
    int situations[20][20];  //记录棋局
    int pattern;   //模式设置 1、人机  2、双人  3、电脑
    int gamestart;  //游戏是否开始
    int order = 0;  //人机玩家先手或者后手，先手为1，后手为2

signals:
    void back();
};
#endif // MAINWINDOW_H
