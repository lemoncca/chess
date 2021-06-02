#include "loginwindow.h"
#include "ui_loginwindow.h"

loginwindow::loginwindow(QWidget *parent)
    : QMainWindow(parent)
    ,ui(new Ui::loginwindow)
{
    w = new MainWindow(this);

    ui->setupUi(this);

    musiclist = new QMediaPlaylist(this);
    musiclist->addMedia(QUrl("qrc:/music/1.mp3"));

    soundplayer = new QMediaPlayer(this);
    soundplayer->setPlaylist(musiclist);
    soundplayer->play();

    musiclist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);

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

    connect(ui->actionopen_music,&QAction::triggered,
            [this]()
    {
        soundplayer->play();
    }
            );
    connect(ui->actionclose_music,&QAction::triggered,
            [this]()
    {
        soundplayer->stop();
    }
            );

    connect(ui->actionrules,&QAction::triggered,
            [this]()
    {
        QMessageBox::information(this,"规则说明","1、胜负：率先连成六子的玩家获胜。\n2、先后手顺序：黑先白后\n3、禁手规则（仅对黑棋方有效）\n     当下完一步棋后，若黑棋方形成两个活四，\n     "
                                             "或两个活五/两个冲五/一活五、一冲五,则\n     为黑棋方犯规，黑棋方出局。");
    }
            );

    connect(ui->actiondescription,&QAction::triggered,
            [this]()
    {
        QMessageBox::information(this,"关于本项目","1、实现本地人机、机机、双人对战\n2、实现联网对战（c v s、c v c，含agent）\n"
                                            "3、项目优势：\n      实现全部要求功能；界面友好；\n      机器算法强度尚可；发现漏洞全部修复。\n"
                                            "4、项目不足：\n      由于无要求，随机匹配对战尚未开发；\n      菜单栏等尚有美化空间。");
    }
            );

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
