#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <mainwindow.h>      //将游戏界面作为登录界面的子对象

namespace Ui {
class loginwindow;
}

class loginwindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit loginwindow(QWidget *parent = nullptr);
    ~loginwindow();

    void player_computer();  //四种对战模式
    void player_player();
    void computer_computer();
    void server_client();
    void client_client();

    void deal_back();   //处理游戏界面发回的信号

private:
    Ui::loginwindow *ui;
    MainWindow *w;         //游戏界面为子对象

signals:

};

#endif // LOGINWINDOW_H
