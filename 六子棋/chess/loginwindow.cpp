#include "loginwindow.h"
#include "ui_loginwindow.h"

loginwindow::loginwindow(QWidget *parent)
    : QMainWindow(parent)
    ,ui(new Ui::loginwindow)
{
    w = new MainWindow(this);

    ui->setupUi(this);

    ui->pushButton->setStyleSheet("QPushButton{border-image:url(://image/3.jpg);font: 20pt KaiTi;}"
                                  "QPushButton:hover{border-image:url(://image/2.jpg)}");
    ui->pushButton_2->setStyleSheet("QPushButton{border-image:url(://image/3.jpg);font: 18pt KaiTi;}"
                                  "QPushButton:hover{border-image:url(://image/2.jpg)}");
    ui->pushButton_3->setStyleSheet("QPushButton{border-image:url(://image/3.jpg);font: 20pt KaiTi;}"
                                  "QPushButton:hover{border-image:url(://image/2.jpg)}");
    ui->pushButton_cc->setStyleSheet("QPushButton{border-image:url(://image/3.jpg);font: 15pt KaiTi;}"
                                      "QPushButton:hover{border-image:url(://image/2.jpg)}");
    ui->pushButton_cs->setStyleSheet("QPushButton{border-image:url(://image/3.jpg);font: 15pt KaiTi;}"
                                      "QPushButton:hover{border-image:url(://image/2.jpg)}");


    this->setWindowTitle("登陆");     //设置标题
    this->setFixedSize(1300,900);     //设置大小
    this->setStyleSheet("QMainWindow{border-image:url(://image/1.jpg)}");   //设置背景

    connect(ui->pushButton,&QPushButton::clicked,   //三种对战模式的信号
            this,&loginwindow::player_computer);

    connect(ui->pushButton_2,&QPushButton::clicked,
            this,&loginwindow::player_player);

    connect(ui->pushButton_3,&QPushButton::clicked,
            this,&loginwindow::computer_computer);

    connect(ui->pushButton_cc,&QPushButton::clicked,
            this,&loginwindow::client_client);

    connect(ui->pushButton_cs,&QPushButton::clicked,
            this,&loginwindow::server_client);

    connect(w,&MainWindow::back,this,&loginwindow::deal_back);  //接收到游戏界面的返回信号后，处理返回信号

}

loginwindow::~loginwindow()
{
    delete ui;
}

void loginwindow::player_computer()   //人机
{
    w->show();
    w->set_pattern(1);
    this->hide();
}

void loginwindow::player_player()   //双人
{
    w->show();
    w->set_pattern(2);
    this->hide();
}

void loginwindow::computer_computer()   //电脑对决
{
    w->show();
    w->set_pattern(3);
    this->hide();
}

void loginwindow::server_client()   //服务端客户端对决
{
    w->show();
    w->set_pattern(4);
    this->hide();
}

void loginwindow::client_client()   //客户端客户端对决
{
    w->show();
    w->set_pattern(5);
    this->hide();
}

void loginwindow::deal_back()   //处理游戏界面发回的返回菜单信号
{
    w->reset_all();
    w->hide();
    w->is_backed_menu();

    this->show();
}
