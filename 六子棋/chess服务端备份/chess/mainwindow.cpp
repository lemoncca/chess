#include "mainwindow.h"
#include "ui_mainwindow.h"

int MainWindow::pattern = 0;
int MainWindow::gamestart = 0;
int MainWindow::X = 0;
int MainWindow::Y = 0;
int MainWindow::times = 0;
int MainWindow::order = 0;
int MainWindow::is_listen = 0;
int MainWindow::situations[20][20] = {{0}};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    msg = new MymessageBox(this);
    msg->setFixedSize(300,120);

    server = new QTcpServer(this);

    Robot_thread = new robot_thread(this);

    //    机器人线程专门用于人机对战
    //    目的是为了实现异步交互的功能

    connect(Robot_thread,&robot_thread::robot_final_done,this,&MainWindow::deal_robot_final_done);
    connect(this,&MainWindow::destroyed,this,&MainWindow::reset_all);     //关闭窗口重置变量，防止线程一直运行
    connect(this,&MainWindow::destroyed,this,&MainWindow::stop_thread);     //关闭窗口时停止线程

    connect(Robot_thread,&robot_thread::robot_done,this,&MainWindow::deal_robot_done);      //人机对战结束时停止线程

    connect(ui->pushButton_4,&QPushButton::clicked,this,&MainWindow::backmenu);  //返回主菜单
    connect(ui->pushButton_5,&QPushButton::clicked,    //点击开始游戏，六子棋开始
            [this]()
    {
        if(gamestart == 1 || (pattern == 4 && client_num < 1) || (pattern == 5 && client_num < 2))
        {
            return;
        }
        gamestart = 1;                  //游戏状态设置为开始
        if(pattern == 4 && is_listen && client_num >= 1)
        {
            QTime time;
            time = QTime::currentTime();
            qsrand(time.msec() + time.second());
            int n = qrand() % 10;
            if(n >= 5)
            {
                order = 1;

                msg->setWindowTitle("先后手?");
                msg->setText("你抽到了先手！");
                msg->show();
            }
            else
            {
                order = 2;

                msg->setWindowTitle("先后手?");
                msg->setText("你抽到了后手！");
                msg->show();
            }
            if(order == 2)
            {
                QString str = "order1";
                socket = tcpsocket_list[0];
                socket->write(str.toUtf8().data());
            }
            else if(order == 1)
            {
                QString str = "order2";
                socket = tcpsocket_list[0];
                socket->write(str.toUtf8().data());
            }
        }


        //                ui->pushButton_5->setText("对弈中");    //游戏开始后，更改文字，提醒用户

        if(pattern == 1)
        {

            msg->setWindowTitle("robot_thread");
            msg->setText("               AI  线程 \n (用于实现异步交互功能要求) \n                  启 动!");
            msg->show();

            Robot_thread->start();
        }

        //                if(pattern == 1 && order == 2)  //若是人后手，先给中央下一个子
        //                {
        //                    situations[10][10] = 1;
        //                    times++;
        //                    update();
        //                }
    }
    );
    connect(ui->pushButton_6,&QPushButton::clicked,    //人机先手
            [this]()
    {
        order = 1;
        update();
    }
    );
    connect(ui->pushButton_7,&QPushButton::clicked,    //人机后手
            [this]()
    {
        order = 2;
        update();
    }
    );
    connect(ui->pushButton_8,&QPushButton::clicked,     //观看电脑下棋，按一次“下一步”，电脑下一步
            [this]()
    {
        gamestart = 1;
        times++;
        int temp = robot( (times-1) % 2 + 1 );
        X = temp / 100;
        Y = temp % 100;
        update();
        judge_win(X,Y);
    }
    );
    connect(ui->pushButton_9,&QPushButton::clicked,     //重新开始棋局（不返回主界面）
            [this]()
    {
        is_to_restart = 1;
        reset_all();
    }
    );
    connect(ui->pushButton_listen,&QPushButton::clicked,this,&MainWindow::start_listen);
    connect(server,&QTcpServer::newConnection,this,&MainWindow::server_new_connect);
    connect(msg,&QMessageBox::buttonClicked,this,&MainWindow::judge_whether_reset_when_msg_close);
    connect(msg,&MymessageBox::msg_is_closed,this,&MainWindow::judge_whether_reset_when_msg_close);


    for (int m = 0;m < 20;m++)      //初始化棋盘为无棋子状态
    {
        for (int n = 0;n <20;n++)
        {
            situations[m][n] = 0;
        }
    }
    pattern = 0;  //模式初始为没有
    gamestart = 0;  //游戏设置为未开始状态

    this->setWindowTitle("游戏界面");   //设置标题
    this->setFixedSize(1300,900);      //固定大小
    this->setStyleSheet("QMainWindow{border-image:url(://image/2.jpg)}");  //设置背景

    ui->pushButton_4->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"       //按钮样式设置
                                    "QPushButton:hover{border-image:url(://image/5.jpg)}");
    ui->pushButton_5->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"
                                    "QPushButton:hover{border-image:url(://image/5.jpg)}");
    ui->pushButton_6->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"
                                    "QPushButton:hover{border-image:url(://image/5.jpg)}");
    ui->pushButton_7->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"
                                    "QPushButton:hover{border-image:url(://image/5.jpg)}");
    ui->pushButton_8->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"
                                    "QPushButton:hover{border-image:url(://image/5.jpg)}");
    ui->pushButton_9->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"
                                    "QPushButton:hover{border-image:url(://image/5.jpg)}");
    ui->pushButton_listen->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"
                                         "QPushButton:hover{border-image:url(://image/5.jpg)}");


    this->setMouseTracking(true);  //设置自动追踪鼠标
    ui->centralwidget->setMouseTracking(true);  //设置自动追踪鼠标
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::judge_whether_reset_when_msg_close()
{
    if(whether_reset_when_msg_close == 0)
    {
        return;
    }
    else
    {
        whether_reset_when_msg_close = 0;
        reset_all();
    }
}

void MainWindow::is_backed_menu()
{
    is_to_back_menu = 0;
}

void MainWindow::reset_all()  //初始化
{
    gamestart = 0;          //不用返回主界面，就可以继续下
    for (int m = 0;m < 20;m++)      //重新初始化棋盘为无棋子状态
    {
        for (int n = 0;n <20;n++)
        {
            situations[m][n] = 0;
        }
    }
    order = 0;
    times = 0;              //注意：此处模式（pattern）没有改变！为了让用户下完一盘后，不用返回主界面就能继续下

    if((pattern == 4 || pattern == 5) && is_listen && is_to_restart )
    {
        foreach(QTcpSocket *i,tcpsocket_list)
        {
            socket = i;
            socket->write(QString("reset").toUtf8().data());
        }
        is_to_restart = 0;
    }
    else if((pattern == 4 || pattern == 5) && is_listen && is_to_back_menu)
    {
        socket_disconnected();

        msg->setWindowTitle("监听提醒");
        msg->setText("监听已经结束！");
        msg->show();

        server->close();
        is_listen = 0;
    }

    update();
}

void MainWindow::set_pattern(int p)
{
    this->pattern = p;          //设置下棋模式
}

void MainWindow::backmenu()  //返回主菜单，发送信号
{
    is_to_back_menu = 1;
    emit back();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    if(pattern == 3)    //如果是电脑对战，只显示“下一步”，不显示“开始游戏”以及“先手”、“后手”
    {
        ui->pushButton_8->show();
        ui->pushButton_5->hide();
        ui->pushButton_6->hide();
        ui->pushButton_7->hide();
        ui->pushButton_listen->hide();
    }
    else if(pattern == 1 && order == 0)          //判断是否是人机对战且还未决定先后手，如果是，那么先选择先手还是后手
    {
        ui->pushButton_5->hide();
        ui->pushButton_6->show();
        ui->pushButton_7->show();
        ui->pushButton_8->hide();
        ui->pushButton_listen->hide();
    }
    else if((pattern == 4 || pattern == 5) && is_listen == 0)
    {
        ui->pushButton_5->hide();
        ui->pushButton_6->hide();
        ui->pushButton_7->hide();
        ui->pushButton_8->hide();
        ui->pushButton_listen->show();
    }
    else if((pattern == 4 || pattern == 5) && is_listen == 1)
    {
        ui->pushButton_5->show();
        ui->pushButton_6->hide();
        ui->pushButton_7->hide();
        ui->pushButton_8->hide();
        ui->pushButton_listen->hide();
    }
    else                              //如果是玩家对战，或者是人机对战中已经决定了先后手，那么只显示“开始游戏”
    {
        ui->pushButton_5->show();
        ui->pushButton_6->hide();
        ui->pushButton_7->hide();
        ui->pushButton_8->hide();
        ui->pushButton_listen->hide();
    }

    if(gamestart == 0 && pattern != 4 && pattern != 5)          //游戏未开始时，按钮显示“开始游戏”
    {
        ui->pushButton_5->setText("开始游戏");
    }
    else if(gamestart == 1 && pattern != 4 && pattern != 5)
    {
        ui->pushButton_5->setText("对弈中");
    }
    else if((pattern == 4 && client_num < 1) && is_listen)
    {
        ui->pushButton_5->setText("等待用户加入");
    }
    else if((pattern == 5 && client_num < 2) && is_listen)
    {
        ui->pushButton_5->setText("等待用户加入");
    }
    else if(pattern == 5 && client_num >= 2 && is_listen && order != 0)
    {
        ui->pushButton_5->setText("观战中");
    }
    else if(pattern == 4 && client_num >= 1 && is_listen && order == 0)
    {
        ui->pushButton_5->setText("抽取顺序");
    }
    else if(pattern == 4 && client_num >= 1 && is_listen && order != 0)
    {
        ui->pushButton_5->setText("对弈中");
    }

    QPainter painter(this);   //创建画家

    QPen pen;   //创建画笔

    pen.setWidth(3);  //画笔宽度
    painter.setPen(pen);  //交画笔
    painter.drawRect(50,50,800,800); //画边框

    pen.setWidth(1);   //画笔宽度调整
    painter.setPen(pen);  //交画笔

    for (int i = 0;i < 19;i++)    //设置格线
    {
        painter.drawLine(50+(i+1)*40,50,50+(i+1)*40,850);
        painter.drawLine(50,50+(i+1)*40,850,50+(i+1)*40);
    }

    if(times == 0) return;   //无棋子时不打印棋子

    for (int m = 1;m < 20;m++)      //刷新棋盘
    {
        for (int n = 1;n <20;n++)
        {
            if(situations[m][n] == 1)       //如果该点为1，打印黑棋
            {
                painter.drawPixmap(m * 40 + 32,n * 40 + 32,37,37,QPixmap("chessimage/3.jpg"));
            }
            else if(situations[m][n] == 2)  //该点为2，打印白棋
            {
                painter.drawPixmap(m * 40 + 32,n * 40 + 32,37,37,QPixmap("chessimage/4.jpg"));
            }
        }
    }

}

void MainWindow::mousePressEvent(QMouseEvent *ev)
{
    if(gamestart == 0 || (pattern == 4 && (client_num < 1 || is_listen == 0)) || pattern == 5)  //若棋局未开始或网络对战不符合要求，不捕捉鼠标按下动作
    {
        return;
    }

    if(pattern == 1 && order == 1)     //人机先手
    {
        if(times % 2 == 1)
        {
            return;
        }

        int x = ev->x();   //获取相对坐标
        int y = ev->y();

        X = (x + 10) / 40 - 1;  //获取点坐标（数组下标）
        Y = (y + 10) / 40 - 1;

        if(70 <= x && x < 830 && y >= 70 && y < 830 && situations[X][Y] == 0) //确保点击区域不越界，且该点还没有棋子
        {
            times++;    //此时下棋总步数加一
            situations[X][Y] = 1;    //黑棋用1代替

            update();  //调用paintevent，刷新棋盘

            int r = 0;
            if(times % 2 == 1)    //黑棋下棋时，判断禁手
            {
                r = judge_rule(X,Y);
                if(r == 5)
                {
                    msg->setWindowTitle("胜负");
                    msg->setText("黑棋违反五五禁手，白棋赢！");
                    whether_reset_when_msg_close = 1;
                    msg->show();
                    return;
                }
                else if(r == 4)
                {
                    msg->setWindowTitle("胜负");
                    msg->setText("黑棋违反四四禁手，白棋赢！");
                    whether_reset_when_msg_close = 1;
                    msg->show();
                    return;
                }
            }

            int win = 0;    //是否胜利

            if(!r)     //若未触犯规则，判断输赢
            {
                win = judge_win(X,Y);  //将判断输赢结果保存在win中
            }

            if(win)         //如果赢了或犯规输掉，就不继续进行机器下棋
            {
                return;
            }

            //            QElapsedTimer time;    //人下完之后暂停一会儿再调用机器，显得更美观
            //            time.start();
            //            while(time.elapsed() < 300)
            //            {
            //                QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            //            }


            //            int temp = robot(2);  //机器下棋，temp中保存三位或者四位数，例如1010，代表10行10列
            //            X = temp / 100;
            //            Y = temp % 100;
            //            times++;
            //            update();  //调用paintevent，刷新棋盘
            //            judge_win(X,Y);
        }
    }
    else if(pattern == 1 && order == 2)     //人机后手
    {
        if(times % 2 == 0)
        {
            return;
        }

        int x = ev->x();   //获取相对坐标
        int y = ev->y();

        X = (x + 10) / 40 - 1;  //获取点坐标（数组下标）
        Y = (y + 10) / 40 - 1;

        if(70 <= x && x < 830 && y >= 70 && y < 830 && situations[X][Y] == 0) //确保点击区域不越界，且该点还没有棋子
        {
            times++;    //此时下棋总步数加一
            situations[X][Y] = 2;    //黑棋用1代替

            update();  //调用paintevent，刷新棋盘
            int win = judge_win(X,Y);

            if(win)
            {
                return;
            }

            //            QElapsedTimer time;    //人下完之后暂停一会儿再调用机器，显得更美观
            //            time.start();
            //            while(time.elapsed() < 300)
            //            {
            //                QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            //            }


            //            int temp = robot(1);  //机器下棋，此时虽然机器黑棋，但是机器自动避免禁手，所以不用判断
            //            X = temp / 100;
            //            Y = temp % 100;
            //            times++;
            //            update();  //调用paintevent，刷新棋盘
            //            judge_win(X,Y);
        }
    }
    else if(pattern == 2)    //人人对战，双方都捕捉鼠标动作
    {
        int x = ev->x();   //获取相对坐标
        int y = ev->y();

        X = (x + 10) / 40 - 1;  //获取点坐标（数组下标）
        Y = (y + 10) / 40 - 1;

        if(70 <= x && x < 830 && y >= 70 && y < 830 && situations[X][Y] == 0) //确保点击区域不越界，且该点还没有棋子
        {
            times++;    //此时下棋总步数加一

            if(times % 2 == 1)  //步数为奇数时，是黑棋
            {
                situations[X][Y] = 1;    //黑棋用1代替
            }
            else if(times % 2 == 0)
            {
                situations[X][Y] = 2;    //白棋用2代替
            }

            update();  //调用paintevent，刷新棋盘

            int r = 0;
            if(times % 2 == 1)    //黑棋下棋时，判断禁手
            {
                r = judge_rule(X,Y);
                if(r == 5)
                {
                    msg->setWindowTitle("胜负");
                    msg->setText("黑棋违反五五禁手，白棋赢！");
                    whether_reset_when_msg_close = 1;
                    msg->show();
                    return;
                }
                else if(r == 4)
                {
                    msg->setWindowTitle("胜负");
                    msg->setText("黑棋违反四四禁手，白棋赢！");
                    whether_reset_when_msg_close = 1;
                    msg->show();
                    return;
                }
            }

            if(!r)     //若未触犯规则，判断输赢
            {
                judge_win(X,Y);
            }
        }
    }
    else if(pattern == 4 && is_listen == 1 && client_num >= 1)
    {
        if(times % 2 == order - 1)
        {
            int x = ev->x();   //获取相对坐标
            int y = ev->y();

            X = (x + 10) / 40 - 1;  //获取点坐标（数组下标）
            Y = (y + 10) / 40 - 1;

            if(70 <= x && x < 830 && y >= 70 && y < 830 && situations[X][Y] == 0) //确保点击区域不越界，且该点还没有棋子
            {
                times++;    //此时下棋总步数加一

                if(times % 2 == 1)  //步数为奇数时，是黑棋
                {
                    situations[X][Y] = 1;    //黑棋用1代替
                }
                else if(times % 2 == 0)
                {
                    situations[X][Y] = 2;    //白棋用2代替
                }

                socket = tcpsocket_list[0];
                QString str = "step:";
                str += QString::number(X) + ' ' + QString::number(Y) ;
                socket->write(str.toUtf8().data());

                update();  //调用paintevent，刷新棋盘

                int r = 0;
                if(times % 2 == 1)    //黑棋下棋时，判断禁手
                {
                    r = judge_rule(X,Y);
                    if(r == 5)
                    {
                        QString str = "ban5";
                        socket = tcpsocket_list[0];
                        socket->write(str.toUtf8().data());

                        msg->setWindowTitle("胜负");
                        msg->setText("黑棋违反五五禁手，白棋赢！");
                        whether_reset_when_msg_close = 1;
                        client_num = 0;
                        update();
                        msg->show();
                        return;
                    }
                    else if(r == 4)
                    {
                        QString str = "ban4";
                        socket = tcpsocket_list[0];
                        socket->write(str.toUtf8().data());

                        msg->setWindowTitle("胜负");
                        msg->setText("黑棋违反四四禁手，白棋赢！");
                        whether_reset_when_msg_close = 1;
                        client_num = 0;
                        update();
                        msg->show();
                        return;
                    }
                }

                if(!r)     //若未触犯规则，判断输赢
                {
                    judge_win(X,Y);
                }
            }
        }
    }
}




