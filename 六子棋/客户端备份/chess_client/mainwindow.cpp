#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    msg = new mymsgbox(this);
    msg->setFixedSize(300,120);

    tcpsocket = new QTcpSocket(this);

    connect_widget = new tcp_connect(this);

    this->setWindowTitle("游戏界面");   //设置标题
    this->setFixedSize(1300,900);      //固定大小
    this->setStyleSheet("QMainWindow{border-image:url(://image/3.jpg)}");  //设置背景

    ui->pushbuttonfind_player->hide();
    ui->pushbuttonfind_player->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"       //按钮样式设置
                                             "QPushButton:hover{border-image:url(://image/5.jpg)}");
    ui->pushbuttondisconnect->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"
                                            "QPushButton:hover{border-image:url(://image/5.jpg)}");
    ui->pushButton_order->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"
                                        "QPushButton:hover{border-image:url(://image/5.jpg)}");
    ui->pushButton_pattern1->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"
                                        "QPushButton:hover{border-image:url(://image/5.jpg)}");
    ui->pushButton_pattern2->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"
                                        "QPushButton:hover{border-image:url(://image/5.jpg)}");


    connect(ui->pushButton_pattern1,&QPushButton::clicked,
            [this]()
    {
        pattern = 1;
        ui->pushbuttonfind_player->show();
        ui->pushButton_pattern1->hide();
        ui->pushButton_pattern2->hide();
    }
            );

    connect(ui->pushButton_pattern2,&QPushButton::clicked,
            [this]()
    {
        pattern = 2;
        ui->pushbuttonfind_player->show();
        ui->pushButton_pattern1->hide();
        ui->pushButton_pattern2->hide();
    }
            );

    connect(connect_widget,&tcp_connect::get_IP_Port,this,&MainWindow::connect_to_server);
    connect(ui->pushbuttonfind_player,&QPushButton::clicked,this,&MainWindow::start_connect);
    connect(tcpsocket,&QTcpSocket::readyRead,this,&MainWindow::read_data_from_server);
    connect(ui->pushbuttondisconnect,&QPushButton::clicked,this,&MainWindow::disconnect_from_server);
    connect(msg,&QMessageBox::buttonClicked,this,&MainWindow::judge_whether_reset_when_msg_close);
    connect(msg,&mymsgbox::msg_is_closed,this,&MainWindow::judge_whether_reset_when_msg_close);

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
        disconnect_from_server();
    }
}

void MainWindow::disconnect_from_server()
{
    tcpsocket->disconnectFromHost();
    tcpsocket->close();
    is_connected = 0;
    is_over = 1;
    reset_all();
    update();

    msg->setWindowTitle("断开连接提醒");
    msg->setText("由于对局结束，或者某用户退出，\n或者服务器停止服务\n你已经与服务端断开连接！");
    msg->show();
}

void MainWindow::reset_all()
{
    for (int m = 0;m < 20;m++)      //重新初始化棋盘为无棋子状态
    {
        for (int n = 0;n <20;n++)
        {
            situations[m][n] = 0;
        }
    }
    order = 0;
    times = 0;
    if(is_over)
    {
        is_over = 0;
        pattern = 0;
    }
}

void MainWindow::read_data_from_server()
{
    QByteArray buffer = tcpsocket->readAll();
    qDebug() << buffer;
    for(int i = 0;i < buffer.length();)
    {
        if(buffer[i+0] == 'o' && buffer[i+1] == 'r' && buffer[i+2] == 'd' && buffer[i+3] == 'e' && buffer[i+4] == 'r')
        {
            if(buffer[i+5] == '1')
            {
                order = 1;
            }
            else if(buffer[i+5] == '2')
            {
                order = 2;
            }


            if(order == 1 && pattern == 2)
            {
                int index = robot(1);
                repaint();
                QString str = QString::number(index / 100);
                str += ' ';
                str += QString::number(index % 100);
                tcpsocket->write(str.toUtf8().data());
            }

            i += 6;
            continue;
        }
        else if(buffer[i+0] == 'w' && buffer[i+1] == 'i' && buffer[i+2] == 'n')
        {
            if(buffer[i+3] == 'w')
            {
                //                QMessageBox::information(this,QString("胜负"),QString("对战结束，白棋赢！\n将自动退出!"));
                msg->setWindowTitle("胜负");
                msg->setText("对战结束，白棋赢！\n将自动退出!");
                whether_reset_when_msg_close = 1;
                is_over = 1;
                msg->show();
            }
            else if(buffer[i+3] == 'b')
            {
                //                QMessageBox::information(this,QString("胜负"),QString("对战结束，黑棋赢！\n将自动退出!"));
                msg->setWindowTitle("胜负");
                msg->setText("对战结束，黑棋赢！\n将自动退出!");
                whether_reset_when_msg_close = 1;
                is_over = 1;
                msg->show();
            }
            else if(buffer[i+3] == 'n')
            {
                //                QMessageBox::information(this,QString("胜负"),QString("对战结束，和棋！\n将自动退出!"));
                msg->setWindowTitle("胜负");
                msg->setText("对战结束，和棋！\n将自动退出!");
                whether_reset_when_msg_close = 1;
                is_over = 1;
                msg->show();
            }

            update();
            i += 4;
            continue;
        }
        else if(buffer[i+0] == 's' && buffer[i+1] == 't' && buffer[i+2] == 'e' && buffer[i+3] == 'p' && buffer[i+4] == ':')
        {
            int color = 0;
            if(order == 1)  color = 2;
            else if(order == 2) color = 1;

            int len;

            if(buffer[i+8] != ' ' && (buffer[i+8] < '0' || buffer[i+8] > '9') )
            {
                len = 8;
            }
            else if(buffer[i+9] != ' ' && (buffer[i+9] < '0' || buffer[i+9] > '9') )
            {
                len = 9;
            }
            else  /*   if(buffer[i+10] != ' ' && (buffer[i+10] < 'a' || buffer[i+10] > 'z') )  */
            {
                len = 10;
            }


            if(len == 8)
            {
                X = buffer[i+5] - '0';
                Y = buffer[i+7] - '0';
                times++;
                situations[X][Y] = color;
                i += 8;
            }
            else if(len == 9)
            {
                if(buffer[i+6] == ' ')
                {
                    X = buffer[i+5] - '0';
                    Y = buffer[i+7] - '0';
                    Y = Y * 10 + buffer[i+8] - '0';
                    times++;
                    situations[X][Y] = color;
                }
                else if(buffer[i+7] == ' ')
                {
                    X = buffer[i+5] - '0';
                    X = X * 10 + buffer[i+6] - '0';
                    Y = buffer[i+8] - '0';
                    times++;
                    situations[X][Y] = color;
                }
                i += 9;
            }
            else if(len == 10)
            {
                X = buffer[i+5] - '0';
                X = X * 10 + buffer[i+6] - '0';
                Y = buffer[i+8] - '0';
                Y = Y * 10 + buffer[i+9] - '0';
                times++;
                situations[X][Y] = color;
                i += 10;
            }

            repaint();

            if(order == 1 && pattern == 2)
            {
                QElapsedTimer time;    //人下完之后暂停一会儿再调用机器，显得更美观
                time.start();
                while(time.elapsed() < 900)     {}
                int index = robot(1);
                times++;
                QString str = QString::number(index / 100);
                str += ' ';
                str += QString::number(index % 100);
                tcpsocket->write(str.toUtf8().data());
                update();
            }
            else if(order == 2 && pattern == 2)
            {
                QElapsedTimer time;    //人下完之后暂停一会儿再调用机器，显得更美观
                time.start();
                while(time.elapsed() < 900)     {}
                int index = robot(2);
                times++;
                QString str = QString::number(index / 100);
                str += ' ';
                str += QString::number(index % 100);
                tcpsocket->write(str.toUtf8().data());
                update();
            }
            continue;
        }
        else if( buffer[i+0] == 'b' && buffer[i+1] == 'a' && buffer[i+2] == 'n' )
        {
            if(buffer[i+3] == '4')
            {
                //                QMessageBox::information(this,QString("胜负"),QString("黑棋违反四四禁手，白棋赢！\n对局结束，将自动退出!"));
                msg->setWindowTitle("胜负");
                msg->setText("黑棋违反四四禁手，白棋赢！\n对局结束，将自动退出!");
                whether_reset_when_msg_close = 1;
                is_over = 1;
                msg->show();
            }
            else  if(buffer[i+3] == '5')
            {
                //                QMessageBox::information(this,QString("胜负"),QString("黑棋违反五五禁手，白棋赢！\n对局结束，将自动退出!"));
                msg->setWindowTitle("胜负");
                msg->setText("黑棋违反五五禁手，白棋赢！\n对局结束，将自动退出!");
                whether_reset_when_msg_close = 1;
                is_over = 1;
                msg->show();
            }

            update();
            i += 4;
            continue;
        }
        else if( buffer[i+0] == 'r' && buffer[i+1] == 'e' && buffer[i+2] == 's'  && buffer[i+3] == 'e' && buffer[i+4] == 't')
        {
            i += 5;
            reset_all();
            update();
            continue;
        }
        else if(buffer[i+0] == 'd' && buffer[i+1] == 'i' && buffer[i+2] == 's'  && buffer[i+3] == 'c' && buffer[i+4] == 'o' &&
                buffer[i+5] == 'n' && buffer[i+6] == 'n' && buffer[i+7] == 'e' && buffer[i+8] == 'c' && buffer[i+9] == 't')
        {
            i += 10;
            disconnect_from_server();
            continue;
        }
        else if(buffer[i+0] == 'n' && buffer[i+1] == 'o' && buffer[i+2] == 'u'  && buffer[i+3] == 's' && buffer[i+4] == 'e')
        {
            i += 5;
            msg->setWindowTitle("暂不支持！");
            msg->setText("抱歉！\n服务端正在与一位客户进行对战，\n您连接后将无法进行任何对战，\n多位客户的混合类型对战仍在开发，\n敬请期待！");
            msg->show();

            tcpsocket->disconnectFromHost();
            tcpsocket->close();
            is_connected = 0;
            reset_all();
            update();

            continue;
        }

        break;
    }

}

void MainWindow::start_connect()
{
    if(is_connected == 0)
    {
        connect_widget->show();
    }
}

void MainWindow::connect_to_server()
{
    QString ip = connect_widget->get_ip();
    quint16 port = connect_widget->get_port();

    //主动和服务器建立连接
    tcpsocket->connectToHost(QHostAddress(ip),port);

    if(!tcpsocket->waitForConnected(3000))
    {
        msg->setWindowTitle("连接超时");
        msg->setText("连接服务端失败！\n请检查端口与IP正确性！\n并检查服务端监听是否开启！");
        msg->show();
    }
    else
    {
        is_connected = 1;
    }

    connect_widget->set_button_text_connect();
    connect_widget->hide();

    update();
}


void MainWindow::paintEvent(QPaintEvent *)
{
    if(pattern == 0)
    {
        ui->pushButton_pattern1->show();
        ui->pushButton_pattern2->show();
        ui->pushbuttonfind_player->hide();
    }

    if(is_connected == 0)
    {
        ui->pushbuttonfind_player->setText("开始寻找对手");
    }
    else if(is_connected == 1)
    {
        ui->pushbuttonfind_player->setText("已经连接");
    }

    if(order == 0)
    {
        ui->pushButton_order->setText("等待抽取顺序");
    }
    else if(order == 1)
    {
        ui->pushButton_order->setText("您使用黑棋");
    }
    else if(order == 2)
    {
        ui->pushButton_order->setText("您使用白棋");
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
    if(pattern == 1)
    {
        if((order == 1 && times % 2 == 0) || (order == 2 && times % 2 == 1))
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

                QString str = QString::number(X);
                str += ' ';
                str += QString::number(Y);
                tcpsocket->write(str.toUtf8().data());

                update();  //调用paintevent，刷新棋盘
            }
        }
    }
}

int MainWindow::judge_rule(int i, int j)   //禁手判断
{
    int a = i,b = j;
    int ban_four = 0,ban_five = 0;      //记录某点有几个活五、活四
    int space = 0;          //空格数
    int temp_count = 0;     //暂时计数器



//        下面同一个方向上有两个循环，第一个循环用于判断活四活五，第二个用于判断冲五

    for (a = i - 5;a <= i;a++)  //假设该串棋的开始点 与 要判断的格子 中间有4（之后的循环中分别假设有3、2、1、0）个格子
    {                           //有四个格子是因为，最大可能格子数为：三个友方棋子和一个空格
        if(a <= 0) a = 1;       //保证循环点大于0

        int m = a;
        while(space <= 1 && m < 20 && temp_count < 5)     //空格数小于2保证能构成一串棋，m < 20保证判断范围不超出棋盘
        {
            if(situations[a-1][j] != 0 || a == 1) break;    //若这串棋的开始点左边不是空格，那么这串棋子的开始点不在这里。于是退出本次假设，之后进入下一次假设（假设开始点前移一位）
            if(situations[a][j] != 1) break;                //若这串棋的开始点不是友方棋子，那么这串棋子的开始点不在这里，假设不成立，退出本次假设，之后处理与上一行相同。

            if(situations[m][j] == 0) space++;              //若该点为0，说明无棋子，空格数加一
            else if(situations[m][j] == 1) temp_count++;    //若该点是友方棋子（由于此处判断禁手只针对黑棋，所以友方棋子就是"1"，即黑棋），临时计数器加一
            else if((situations[m][j] == 2 || m == 19) && situations[m-1][j] == 1)      //若该点是敌方棋子，且左边一点为友方棋子
            {                                                                           //那么该串棋只可能是冲，不构成禁手，退出该循环，临时计数清零
                temp_count = 0;
                break;
            }
            else if((situations[m][j] == 2 || m == 19) && situations[m-1][j] == 0)     //若该点是敌方棋子，且左边一点为空格
            {                                                                          //那么该串棋子可以构成活棋（可能禁手），但是再往右不可能还有棋子
                break;                                                                 //于是直接退出判断是否满足活五或者活四
            }
            m++;        //从该点往右移
        }
        if(temp_count == 5 && situations[m][j] == 0)      //若临时计数器等于五且该串棋子结束点的右边是空的，方可认为这串棋子是活五
        {
            ban_five++;
            temp_count = 0;
            space = 0;
            break;              //如果有了一个活五，那么默认该串棋子不是活四（否则对黑棋过于残忍。。）
        }
        else if(temp_count == 4)
        {
            ban_four++;
        }
        temp_count = 0;         //循环中要用的值归0
        space = 0;
    }

    for (a = i - 5;a <= i;a++)  //冲五判断与活五大致相同，多做了一个if分支判断（内容大致相同），即究竟是左边有白棋还是右边有白棋
    {
        if(a <= 0)
        {
            a = 1;
        }

        int m = a;
        if((situations[a-1][j] == 2 || a == 1) && a != i)
        {
            while(space <= 1 && m < 20)
            {
                if(situations[a][j] != 1)
                {
                    break;
                }

                if(situations[m][j] == 0) space++;
                else if(situations[m][j] == 1) temp_count++;
                else if((situations[m][j] == 2 || m == 19) && situations[m-1][j] == 1)
                {
                    temp_count = 0;
                    break;
                }
                else if((situations[m][j] == 2 || m == 19) && situations[m-1][j] == 0)
                {
                    break;
                }
                m++;
            }
            if(temp_count == 5)
            {
                ban_five++;
            }
            temp_count = 0;
            space = 0;
        }
        else if(situations[a-1][j] == 0 && a != 1)
        {
            while(space <= 1 && m < 20)
            {
                if(situations[a][j] != 1)
                {
                    break;
                }

                if(situations[m][j] == 0) space++;
                else if(situations[m][j] == 1) temp_count++;
                else if((situations[m][j] == 2 || m == 19) && situations[m-1][j] == 1)
                {
                    break;
                }
                else if((situations[m][j] == 2 || m == 19) && situations[m-1][j] == 0)
                {
                    temp_count = 0;
                    break;
                }
                m++;
            }
            if(temp_count == 5 && space <= 1)
            {
                ban_five++;
            }
            temp_count = 0;
            space = 0;
        }
    }



    for (b = j - 5;b <= j;b++)      //竖直方向，同理与水平方向
    {
        if(b <= 0) b = 1;

        int m = b;
        while(space <= 1 && m < 20 && temp_count < 5)
        {
            if(situations[i][b-1] != 0 || b == 1) break;
            if(situations[i][b] != 1) break;

            if(situations[i][m] == 0) space++;
            else if(situations[i][m] == 1) temp_count++;
            else if((situations[i][m] == 2 || m == 19) && situations[i][m-1] == 1)
            {
                temp_count = 0;
                break;
            }
            else if((situations[i][m] == 2 || m == 19) && situations[i][m-1] == 0)
            {
                break;
            }
            m++;
        }
        if(temp_count == 5 && situations[i][m] == 0)
        {
            ban_five++;
            temp_count = 0;
            space = 0;
            break;
        }
        else if(temp_count == 4)
        {
            ban_four++;
        }
        temp_count = 0;
        space = 0;
    }

    for (b = j - 5;b <= j;b++)
    {
        if(b <= 0)
        {
            b = 1;
        }

        int m = b;
        if((situations[i][b-1] == 2 || b == 1) && b != j)
        {
            while(space <= 1 && m < 20)
            {
                if(situations[i][b] != 1)
                {
                    break;
                }

                if(situations[i][m] == 0) space++;
                else if(situations[i][m] == 1) temp_count++;
                else if((situations[i][m] == 2 || m == 19) && situations[i][m-1] == 1)
                {
                    temp_count = 0;
                    break;
                }
                else if((situations[i][m] == 2 || m == 19) && situations[i][m-1] == 0)
                {
                    break;
                }
                m++;
            }
            if(temp_count == 5)
            {
                ban_five++;
            }
            temp_count = 0;
            space = 0;
        }
        else if(situations[i][b-1] == 0 && b != 1)
        {
            while(space <= 1 && m < 20)
            {
                if(situations[i][b] != 1)
                {
                    break;
                }

                if(situations[i][m] == 0) space++;
                else if(situations[i][m] == 1) temp_count++;
                else if((situations[i][m] == 2 || m == 19) && situations[i][m-1] == 1)
                {
                    break;
                }
                else if((situations[i][m] == 2 || m == 19) && situations[i][m-1] == 0)
                {
                    temp_count = 0;
                    break;
                }
                m++;
            }
            if(temp_count == 5 && space <= 1)
            {
                ban_five++;
            }
            temp_count = 0;
            space = 0;
        }
    }




    for (a = i - 5,b = j - 5;a <= i;a++,b++)    //斜方向，同理与水平方向
    {
        while(a <= 0 || b <= 0)
        {
            a++;
            b++;
        }

        int m = a,n = b;
        while(space <= 1 && m < 20 && n < 20 && temp_count < 5)
        {
            if(situations[a-1][b-1] != 0 || a == 1 || b == 1) break;
            if(situations[a][b] != 1) break;

            if(situations[m][n] == 0) space++;
            else if(situations[m][n] == 1) temp_count++;
            else if((situations[m][n] == 2 || m == 19 || n == 19) && situations[m-1][n-1] == 1)
            {
                temp_count = 0;
                break;
            }
            else if((situations[m][n] == 2 || m == 19 || n == 19) && situations[m-1][n-1] == 0)
            {
                break;
            }
            m++;
            n++;
        }
        if(temp_count == 5 && situations[m][n] == 0)
        {
            ban_five++;
            temp_count = 0;
            space = 0;
            break;
        }
        else if(temp_count == 4)
        {
            ban_four++;
        }
        temp_count = 0;
        space = 0;
    }

    for (b = j - 5,a = i - 5;a <= i;a++,b++)
    {
        while(a <= 0 || b <= 0)
        {
            a++;
            b++;
        }

        int m = a,n = b;
        if((situations[a-1][b-1] == 2 || b == 1 || a == 1) && b != j)
        {
            while(space <= 1 && m < 20 && n < 20)
            {
                if(situations[a][b] != 1)
                {
                    break;
                }

                if(situations[m][n] == 0) space++;
                else if(situations[m][n] == 1) temp_count++;
                else if((situations[m][n] == 2 || m == 19 || n == 19) && situations[m-1][n-1] == 1)
                {
                    temp_count = 0;
                    break;
                }
                else if((situations[m][n] == 2 || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                {
                    break;
                }
                m++;
                n++;
            }
            if(temp_count == 5)
            {
                ban_five++;
            }
            temp_count = 0;
            space = 0;
        }
        else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
        {
            while(space <= 1 && m < 20 && n < 20)
            {
                if(situations[a][b] != 1)
                {
                    break;
                }

                if(situations[m][n] == 0) space++;
                else if(situations[m][n] == 1) temp_count++;
                else if((situations[m][n] == 2 || m == 19 || n == 19) && situations[m-1][n-1] == 1)
                {
                    break;
                }
                else if((situations[m][n] == 2 || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                {
                    temp_count = 0;
                    break;
                }
                m++;
                n++;
            }
            if(temp_count == 5 && space <= 1)
            {
                ban_five++;
            }
            temp_count = 0;
            space = 0;
        }
    }




    for (a = i - 5,b = j + 5;a <= i;a++,b--)        //斜方向，同理与水平方向
    {
        while(a <= 0 || b >= 20)
        {
            a++;
            b--;
        }

        int m = a,n = b;
        while(space <= 1 && m < 20 && n > 0 && temp_count < 5)
        {
            if(situations[a-1][b+1] != 0 || a == 1 || b == 19) break;
            if(situations[a][b] != 1) break;

            if(situations[m][n] == 0) space++;
            else if(situations[m][n] == 1) temp_count++;
            else if((situations[m][n] == 2 || m == 19 || n == 1) && situations[m-1][n+1] == 1)
            {
                temp_count = 0;
                break;
            }
            else if((situations[m][n] == 2 || m == 19 || n == 1) && situations[m-1][n+1] == 0)
            {
                break;
            }
            m++;
            n--;
        }
        if(temp_count == 5 && situations[m][n] == 0)
        {
            ban_five++;
            temp_count = 0;
            space = 0;
            break;
        }
        else if(temp_count == 4)
        {
            ban_four++;
        }
        temp_count = 0;
        space = 0;
    }

    for (b = j + 5,a = i - 5;a <= i;a++,b--)
    {
        while(a <= 0 || b >= 20)
        {
            a++;
            b--;
        }

        int m = a,n = b;
        if((situations[a-1][b+1] == 2 || b == 19 || a == 1) && b != j)
        {
            while(space <= 1 && m < 20 && n > 0)
            {
                if(situations[a][b] != 1)
                {
                    break;
                }

                if(situations[m][n] == 0) space++;
                else if(situations[m][n] == 1) temp_count++;
                else if((situations[m][n] == 2 || m == 19 || n == 1) && situations[m-1][n+1] == 1)
                {
                    temp_count = 0;
                    break;
                }
                else if((situations[m][n] == 2 || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                {
                    break;
                }
                m++;
                n--;
            }
            if(temp_count == 5)
            {
                ban_five++;
            }
            temp_count = 0;
            space = 0;
        }
        else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
        {
            while(space <= 1 && m < 20 && n > 0)
            {
                if(situations[a][b] != 1)
                {
                    break;
                }

                if(situations[m][n] == 0) space++;
                else if(situations[m][n] == 1) temp_count++;
                else if((situations[m][n] == 2 || m == 19 || n == 1) && situations[m-1][n+1] == 1)
                {
                    break;
                }
                else if((situations[m][n] == 2 || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                {
                    temp_count = 0;
                    break;
                }
                m++;
                n--;
            }
            if(temp_count == 5 && space <= 1)
            {
                ban_five++;
            }
            temp_count = 0;
            space = 0;
        }
    }



    if(ban_five >= 2)
    {
        return 5;
    }

    else if(ban_four >= 2)
    {
        return 4;
    }
    return 0;
}

int MainWindow::robot(int color)
{
    int weight[20][20] = {{0}};     //权重数组
    int max = 0,max_index = 1010;      //权重最大值
//    int test[18] = {0};           //用于测试ai权值判断准确性，与robot函数最后的qDebug() << test[]  连用

    for(int i = 0;i < 20;i++)       //先初步筛选，等会儿需要判断权重的格子：这些格子的周围有已经下过的格子
    {
        for(int j = 0;j < 20;j++)
        {
            if(situations[i][j] != 0)
            {
                if(i - 1 > 0 && j - 1 > 0)      weight[i-1][j-1] = 1;
                if(i + 1 < 20 && j + 1 < 20)    weight[i+1][j+1] = 1;
                if(i - 1 > 0 && j + 1 < 20)    weight[i-1][j+1] = 1;
                if(i + 1 < 20 && j - 1 > 0)    weight[i+1][j-1] = 1;
                if(i + 1 < 20)      weight[i+1][j] = 1;
                if(i - 1 > 0)       weight[i-1][j] = 1;
                if(j - 1 > 0)       weight[i][j-1] = 1;
                if(j + 1 < 20)      weight[i][j+1] = 1;
            }
        }
    }
    for(int i = 0;i < 20;i++)           //已经下过的格子去掉
    {
        for(int j = 0;j < 20;j++)
        {
            if(situations[i][j] != 0)
            {
                weight[i][j] = 0;
            }
        }
    }

    int enemy;                      //判断敌方是黑棋还是白棋
    if(color == 1)                  //若己方为黑，则敌方为白
    {
        enemy = 2;
    }
    else                            //若己方为白，则敌方为黑
    {
        enemy = 1;
    }

    /*
     * 下面是核心算法，涉及权重算法以及博弈论的相关原理
     * 步骤一、假设该点已经下了友方棋子，即令 situations[i][j] = color（color中存放了当前
     *        友方棋子颜色："1"为黑色，"2"为白色）
     * 步骤二、在假设的前提下，先判断四个方向有没有成六（因为六不用考虑活、冲，较好判断，且六是
     *        决定性的）
     * 步骤三、先判断横着方向的活
     * 步骤四、再判断横着方向的冲
     * 步骤五、接着在横着方向，所有存在的活、冲棋型中，挑选权重最大的一个作为该方向的代表赋入
     *        friend_true数组中，权重大小为：活5 > 活4 > 冲5 > 活3 > 冲4 > 活2 > 冲3 >
     *        冲2
     *
     *          注意：步骤五中 “活4 > 冲5” 是因为这是假设已经下了这一点后构成的棋型，而不是
     *               假设该点是空格时周围构成的棋型
     *
     * 步骤六、对竖直、左斜、右斜方向重复步骤三、四、五
     *
     *      中途小结：此时友方在此点的判断全部存入friend_true数组。下面运用博弈，判断
     *              对于敌方而言，此点的重要性；该结果将会存入enemy_true数组中
     *
     * 步骤七、假设该点已经下了敌方棋子，即令 situations[i][j] = enemy（enemy中存放了当前
     *        敌方棋子颜色："1"为黑色，"2"为白色）
     * 步骤八、将enemy中的值与color中的值暂时调换位置，这样就可以用 判断该点对友方重要性的代
     *        码（即步骤二到步骤六的代码） 来判断该点对于敌方的重要性
     * 步骤九、假设该点下了敌方的棋子，重复步骤二至六，以判断该点对于敌方的重要性
     * 步骤十、enemy与color的值互换--交换回真实值
     * 步骤十一、至此，所有敌方友方判断均已结束，并分别放置在friend_true与enemy_true数组中，
     *         我们对这20种棋型乘以它们在该点处存在的个数，再乘以不同权值，并相加。最终我们会
     *         得到该点的总权值
     *
     *              以上，核心算法结束
     *
     * 注：后又添加了available的算法，在下面有详细说明
    */

    for(int i = 0;i < 20;i++)       //开始判断权重
    {
        for(int j = 0;j < 20;j++)
        {
            if(weight[i][j] == 1)   //如果是之前筛选中的对象，就开始分析该点权重
            {
                situations[i][j] = color;       //先假定该点有了己方棋子，判断若下了该点，我方好处

                int a = i,b = j;

                /*活棋类型*/
                int true_two = 0,true_three = 0,true_four = 0,true_five = 0,true_six = 0;
                /*冲棋类型*/
                int fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

                int space = 0;
                int temp_count = 0;
                int friend_true[9] = {0};       //这里一开始取名字失误了！应该表示所有友方类型。其中存储：如果友方下了这个空点，友方构成多少个活、多少个冲
                int enemy_true[9] = {0};        //这里表示敌方。其中存储：如果敌方下了这个空点，敌方构成多少个活、多少个冲
                /*
                 * [0]表示冲2，[1]表示冲3，[2]表示冲4，[3]表示冲5，[4]表示活2，
                 * [5]表示活3，[6]表示活4，[7]表示活5，[8]表示活6或者冲6（活6冲6没区别）
                */

                int available = 0;    //available变量用于判断该点处向左右是否足够有6个空间
                                      //即表明，该点向左的第一个敌人，到向右的第一个敌人之间
                                      //格数；若格数小于6，那么该点处下棋对于我方无意义（因为
                                      //成不了6）；对于敌方，若我方两个棋子之间间隔小于6，那
                                      //么，敌方下这里也没有意义



                for(a = i;situations[a][j] == color && a >= 1;a--)
                {
                    temp_count++;
                }
                for(a = i+1;situations[a][j] == color && a <= 19;a++)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                for(b = j;situations[i][b] == color && b >= 1;b--)
                {
                    temp_count++;
                }
                for(b = j+1;situations[i][b] == color && b <= 19;b++)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                for(b = j,a = i;situations[a][b] == color && a >= 1 && b >= 1;b--,a--)
                {
                    temp_count++;
                }
                for(b = j + 1,a = i + 1;situations[a][b] == color && a <= 19 && b <= 19;b++,a++)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                for(b = j,a = i;situations[a][b] == color && b >= 1 && a <= 19;b--,a++)
                {
                    temp_count++;
                }
                for(b = j + 1,a = i - 1;situations[a][b] == color && b <= 19 && a >= 1;b++,a--)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                if(true_six > 0)
                {
                    friend_true[8] += true_six;
                    true_six = 0;
                }




                available = 0;
                for (a = i;a < i + 6 && situations[a][j] != enemy && a < 20;a++)
                {
                    available++;
                }
                for (a = i - 1;a > i - 6 && situations[a][j] != enemy && a > 0;a--)
                {
                    available++;
                }
                if(available < 6) goto skip_horizon;

                for (a = i - 5;a <= i;a++)
                {
                    if(a <= 0) a = 1;

                    int m = a;
                    while(space <= 1 && m < 20 && temp_count < 5)
                    {
                        if(situations[a-1][j] != 0 || a == 1) break;
                        if(situations[a][j] != color) break;

                        if(situations[m][j] == 0) space++;
                        else if(situations[m][j] == color && m < 19) temp_count++;
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[m][j] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (a = i - 3;a <= i;a++)
                {
                    if(a <= 0) a = 1;

                    int m = a;
                    while(space <= 1 && m < 20)
                    {
                        if(situations[a-1][j] != 0 || a == 1) break;
                        if(situations[a][j] != color) break;

                        if(situations[m][j] == 0) space++;
                        else if(situations[m][j] == color && m < 19) temp_count++;
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (a = i - 5;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (a = i - 4;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (a = i - 3;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (a = i - 2;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                if(true_five > 0)
                {
                    friend_true[7]++;
                }
                else if(true_four > 0)
                {
                    friend_true[6]++;
                }
                else if(fake_five > 0)
                {
                    friend_true[3]++;
                }
                else if(true_three > 0)
                {
                    friend_true[5]++;
                }
                else if(fake_four > 0)
                {
                    friend_true[2]++;
                }
                else if(true_two > 0)
                {
                    friend_true[4]++;
                }
                else if(fake_three > 0)
                {
                    friend_true[1]++;
                }
                else if(fake_two > 0)
                {
                    friend_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;
skip_horizon:



                available = 0;
                for (b = j;b < j + 6 && situations[i][b] != enemy && b < 20;b++)
                {
                    available++;
                }
                for (b = j - 1;b > j - 6 && situations[i][b] != enemy && b > 0;b--)
                {
                    available++;
                }
                if(available < 6) goto skip_upright;

                for (b = j - 5;b <= j;b++)
                {
                    if(b <= 0) b = 1;

                    int m = b;
                    while(space <= 1 && m < 20 && temp_count < 5)
                    {
                        if(situations[i][b-1] != 0 || b == 1) break;
                        if(situations[i][b] != color) break;

                        if(situations[i][m] == 0) space++;
                        else if(situations[i][m] == color && m < 19) temp_count++;
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[i][m] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (b = j - 3;b <= j;b++)
                {
                    if(b <= 0) b = 1;

                    int m = b;
                    while(space <= 1 && m < 20)
                    {
                        if(situations[i][b-1] != 0 || b == 1) break;
                        if(situations[i][b] != color) break;

                        if(situations[i][m] == 0) space++;
                        else if(situations[i][m] == color && m < 19) temp_count++;
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (b = j - 5;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 4;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 3;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 2;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                if(true_five > 0)
                {
                    friend_true[7]++;
                }
                else if(true_four > 0)
                {
                    friend_true[6]++;
                }
                else if(fake_five > 0)
                {
                    friend_true[3]++;
                }
                else if(true_three > 0)
                {
                    friend_true[5]++;
                }
                else if(fake_four > 0)
                {
                    friend_true[2]++;
                }
                else if(true_two > 0)
                {
                    friend_true[4]++;
                }
                else if(fake_three > 0)
                {
                    friend_true[1]++;
                }
                else if(fake_two > 0)
                {
                    friend_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;
skip_upright:



                available = 0;
                for (b = j,a = i;b < j + 6 && situations[a][b] != enemy && a < 20 && b < 20;a++,b++)
                {
                    available++;
                }
                for (b = j - 1,a = i - 1;b > j - 6 && situations[a][b] != enemy && a > 0 && b > 0;a--,b--)
                {
                    available++;
                }
                if(available < 6) goto skip_left_oblique;

                for (a = i - 5,b = j - 5;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n < 20 && temp_count < 5)
                    {
                        if(situations[a-1][b-1] != 0 || a == 1 || b == 1) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n < 19) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                        {
                            break;
                        }
                        m++;
                        n++;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[m][n] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (a = i - 3,b = j - 3;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n < 20)
                    {
                        if(situations[a-1][b-1] != 0 || a == 1 || b == 1) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n < 19) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                        {
                            break;
                        }
                        m++;
                        n++;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (b = j - 5,a = i - 5;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 4,a = i - 4;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 3,a = i - 3;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 2,a = i - 2;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                if(true_five > 0)
                {
                    friend_true[7]++;
                }
                else if(true_four > 0)
                {
                    friend_true[6]++;
                }
                else if(fake_five > 0)
                {
                    friend_true[3]++;
                }
                else if(true_three > 0)
                {
                    friend_true[5]++;
                }
                else if(fake_four > 0)
                {
                    friend_true[2]++;
                }
                else if(true_two > 0)
                {
                    friend_true[4]++;
                }
                else if(fake_three > 0)
                {
                    friend_true[1]++;
                }
                else if(fake_two > 0)
                {
                    friend_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

skip_left_oblique:



                available = 0;
                for (b = j,a = i;b < j + 6 && situations[a][b] != enemy && b < 20 && a > 0;a--,b++)
                {
                    available++;
                }
                for (b = j - 1,a = i + 1;b > j - 6 && situations[a][b] != enemy && b > 0 && a < 20;a++,b--)
                {
                    available++;
                }
                if(available < 6) goto skip_right_oblique;

                for (a = i - 5,b = j + 5;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n > 0 && temp_count < 5)
                    {
                        if(situations[a-1][b+1] != 0 || a == 1 || b == 19) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n > 1) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                        {
                            break;
                        }
                        m++;
                        n--;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[m][n] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (a = i - 3,b = j + 3;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n > 0)
                    {
                        if(situations[a-1][b+1] != 0 || a == 1 || b == 19) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n > 1) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                        {
                            break;
                        }
                        m++;
                        n--;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (b = j + 5,a = i - 5;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j + 4,a = i - 4;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j + 3,a = i - 3;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j + 2,a = i - 2;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }


                if(true_five > 0)
                {
                    friend_true[7]++;
                }
                else if(true_four > 0)
                {
                    friend_true[6]++;
                }
                else if(fake_five > 0)
                {
                    friend_true[3]++;
                }
                else if(true_three > 0)
                {
                    friend_true[5]++;
                }
                else if(fake_four > 0)
                {
                    friend_true[2]++;
                }
                else if(true_two > 0)
                {
                    friend_true[4]++;
                }
                else if(fake_three > 0)
                {
                    friend_true[1]++;
                }
                else if(fake_two > 0)
                {
                    friend_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

skip_right_oblique:



                situations[i][j] = enemy;
                int swap = enemy;
                enemy = color;
                color = swap;

                for(a = i;situations[a][j] == color && a >= 1;a--)
                {
                    temp_count++;
                }
                for(a = i+1;situations[a][j] == color && a <= 19;a++)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                for(b = j;situations[i][b] == color && b >= 1;b--)
                {
                    temp_count++;
                }
                for(b = j+1;situations[i][b] == color && b <= 19;b++)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                for(b = j,a = i;situations[a][b] == color && a >= 1 && b >= 1;b--,a--)
                {
                    temp_count++;
                }
                for(b = j + 1,a = i + 1;situations[a][b] == color && a <= 19 && b <= 19;b++,a++)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                for(b = j,a = i;situations[a][b] == color && b >= 1 && a <= 19;b--,a++)
                {
                    temp_count++;
                }
                for(b = j + 1,a = i - 1;situations[a][b] == color && b <= 19 && a >= 1;b++,a--)
                {
                    temp_count++;
                }
                if(temp_count >= 6)
                {
                    true_six++;
                }
                temp_count = 0;

                if(true_six > 0)
                {
                    enemy_true[8] += true_six;
                    true_six = 0;
                }


                available = 0;
                for (a = i;a < i + 6 && situations[a][j] != enemy && a < 20;a++)
                {
                    available++;
                }
                for (a = i - 1;a > i - 6 && situations[a][j] != enemy && a > 0;a--)
                {
                    available++;
                }
                if(available < 6) goto enemy_skip_horizon;

                for (a = i - 5;a <= i;a++)
                {
                    if(a <= 0) a = 1;

                    int m = a;
                    while(space <= 1 && m < 20 && temp_count < 5)
                    {
                        if(situations[a-1][j] != 0 || a == 1) break;
                        if(situations[a][j] != color) break;

                        if(situations[m][j] == 0) space++;
                        else if(situations[m][j] == color && m < 19) temp_count++;
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[m][j] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (a = i - 3;a <= i;a++)
                {
                    if(a <= 0) a = 1;

                    int m = a;
                    while(space <= 1 && m < 20)
                    {
                        if(situations[a-1][j] != 0 || a == 1) break;
                        if(situations[a][j] != color) break;

                        if(situations[m][j] == 0) space++;
                        else if(situations[m][j] == color && m < 19) temp_count++;
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (a = i - 5;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (a = i - 4;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (a = i - 3;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (a = i - 2;a <= i;a++)
                {
                    if(a <= 0)
                    {
                        a = 1;
                    }

                    int m = a;
                    if((situations[a-1][j] == enemy || a == 1) && a != i)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][j] == 0 && a != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[a][j] != color)
                            {
                                break;
                            }

                            if(situations[m][j] == 0) space++;
                            else if(situations[m][j] == color) temp_count++;
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == color)
                            {
                                break;
                            }
                            else if((situations[m][j] == enemy || m == 19) && situations[m-1][j] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                if(true_five > 0)
                {
                    enemy_true[7]++;
                }
                else if(true_four > 0)
                {
                    enemy_true[6]++;
                }
                else if(fake_five > 0)
                {
                    enemy_true[3]++;
                }
                else if(true_three > 0)
                {
                    enemy_true[5]++;
                }
                else if(fake_four > 0)
                {
                    enemy_true[2]++;
                }
                else if(true_two > 0)
                {
                    enemy_true[4]++;
                }
                else if(fake_three > 0)
                {
                    enemy_true[1]++;
                }
                else if(fake_two > 0)
                {
                    enemy_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

enemy_skip_horizon:



                available = 0;
                for (b = j;b < j + 6 && situations[i][b] != enemy && b < 20;b++)
                {
                    available++;
                }
                for (b = j - 1;b > j - 6 && situations[i][b] != enemy && b > 0;b--)
                {
                    available++;
                }
                if(available < 6) goto enemy_skip_upright;

                for (b = j - 5;b <= j;b++)
                {
                    if(b <= 0) b = 1;

                    int m = b;
                    while(space <= 1 && m < 20 && temp_count < 5)
                    {
                        if(situations[i][b-1] != 0 || b == 1) break;
                        if(situations[i][b] != color) break;

                        if(situations[i][m] == 0) space++;
                        else if(situations[i][m] == color && m < 19) temp_count++;
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[i][m] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (b = j - 3;b <= j;b++)
                {
                    if(b <= 0) b = 1;

                    int m = b;
                    while(space <= 1 && m < 20)
                    {
                        if(situations[i][b-1] != 0 || b == 1) break;
                        if(situations[i][b] != color) break;

                        if(situations[i][m] == 0) space++;
                        else if(situations[i][m] == color && m < 19) temp_count++;
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                        {
                            break;
                        }
                        m++;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (b = j - 5;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 4;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 3;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 2;b <= j;b++)
                {
                    if(b <= 0)
                    {
                        b = 1;
                    }

                    int m = b;
                    if((situations[i][b-1] == enemy || b == 1) && b != j)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[i][b-1] == 0 && b != 1)
                    {
                        while(space <= 1 && m < 20)
                        {
                            if(situations[i][b] != color)
                            {
                                break;
                            }

                            if(situations[i][m] == 0) space++;
                            else if(situations[i][m] == color) temp_count++;
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == color)
                            {
                                break;
                            }
                            else if((situations[i][m] == enemy || m == 19) && situations[i][m-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                if(true_five > 0)
                {
                    enemy_true[7]++;
                }
                else if(true_four > 0)
                {
                    enemy_true[6]++;
                }
                else if(fake_five > 0)
                {
                    enemy_true[3]++;
                }
                else if(true_three > 0)
                {
                    enemy_true[5]++;
                }
                else if(fake_four > 0)
                {
                    enemy_true[2]++;
                }
                else if(true_two > 0)
                {
                    enemy_true[4]++;
                }
                else if(fake_three > 0)
                {
                    enemy_true[1]++;
                }
                else if(fake_two > 0)
                {
                    enemy_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

enemy_skip_upright:



                available = 0;
                for (b = j,a = i;b < j + 6 && situations[a][b] != enemy && a < 20 && b < 20;a++,b++)
                {
                    available++;
                }
                for (b = j - 1,a = i - 1;b > j - 6 && situations[a][b] != enemy && a > 0 && b > 0;a--,b--)
                {
                    available++;
                }
                if(available < 6) goto enemy_skip_left_oblique;

                for (a = i - 5,b = j - 5;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n < 20 && temp_count < 5)
                    {
                        if(situations[a-1][b-1] != 0 || a == 1 || b == 1) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n < 19) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                        {
                            break;
                        }
                        m++;
                        n++;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[m][n] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (a = i - 3,b = j - 3;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n < 20)
                    {
                        if(situations[a-1][b-1] != 0 || a == 1 || b == 1) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n < 19) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                        {
                            break;
                        }
                        m++;
                        n++;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (b = j - 5,a = i - 5;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 4,a = i - 4;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 3,a = i - 3;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j - 2,a = i - 2;a <= i;a++,b++)
                {
                    while(a <= 0 || b <= 0)
                    {
                        a++;
                        b++;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b-1] == enemy || b == 1 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b-1] == 0 && b != 1 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n < 20)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 19) && situations[m-1][n-1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n++;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                if(true_five > 0)
                {
                    enemy_true[7]++;
                }
                else if(true_four > 0)
                {
                    enemy_true[6]++;
                }
                else if(fake_five > 0)
                {
                    enemy_true[3]++;
                }
                else if(true_three > 0)
                {
                    enemy_true[5]++;
                }
                else if(fake_four > 0)
                {
                    enemy_true[2]++;
                }
                else if(true_two > 0)
                {
                    enemy_true[4]++;
                }
                else if(fake_three > 0)
                {
                    enemy_true[1]++;
                }
                else if(fake_two > 0)
                {
                    enemy_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

enemy_skip_left_oblique:


                available = 0;
                for (b = j,a = i;b < j + 6 && situations[a][b] != enemy && b < 20 && a > 0;a--,b++)
                {
                    available++;
                }
                for (b = j - 1,a = i + 1;b > j - 6 && situations[a][b] != enemy && b > 0 && a < 20;a++,b--)
                {
                    available++;
                }
                if(available < 6) goto enemy_skip_right_oblique;

                for (a = i - 5,b = j + 5;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n > 0 && temp_count < 5)
                    {
                        if(situations[a-1][b+1] != 0 || a == 1 || b == 19) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n > 1) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                        {
                            break;
                        }
                        m++;
                        n--;
                    }
                    if(temp_count == 5 && space == 0)
                    {
                        if(situations[m][n] == enemy)   true_five--;
                        true_five++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 4)
                    {
                        true_four++;
                    }
                    temp_count = 0;
                    space = 0;
                }
                for (a = i - 3,b = j + 3;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    while(space <= 1 && m < 20 && n > 0)
                    {
                        if(situations[a-1][b+1] != 0 || a == 1 || b == 19) break;
                        if(situations[a][b] != color) break;

                        if(situations[m][n] == 0) space++;
                        else if(situations[m][n] == color && m < 19 && n > 1) temp_count++;
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                        {
                            temp_count = 0;
                            break;
                        }
                        else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                        {
                            break;
                        }
                        m++;
                        n--;
                    }
                    if(temp_count == 3)
                    {
                        true_three++;
                        temp_count = 0;
                        space = 0;
                        break;
                    }
                    else if(temp_count == 2)
                    {
                        true_two++;
                    }
                    temp_count = 0;
                    space = 0;
                }

                for (b = j + 5,a = i - 5;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 5)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 5 && space <= 1)
                        {
                            fake_five++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j + 4,a = i - 4;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 4)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 4 && space <= 1)
                        {
                            fake_four++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j + 3,a = i - 3;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 3)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 3 && space <= 1)
                        {
                            fake_three++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }

                for (b = j + 2,a = i - 2;a <= i;a++,b--)
                {
                    while(a <= 0 || b >= 20)
                    {
                        a++;
                        b--;
                    }

                    int m = a,n = b;
                    if((situations[a-1][b+1] == enemy || b == 19 || a == 1) && b != j)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                temp_count = 0;
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 2)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                    else if(situations[a-1][b+1] == 0 && b != 19 && a != 1)
                    {
                        while(space <= 1 && m < 20 && n > 0)
                        {
                            if(situations[a][b] != color)
                            {
                                break;
                            }

                            if(situations[m][n] == 0) space++;
                            else if(situations[m][n] == color) temp_count++;
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == color)
                            {
                                break;
                            }
                            else if((situations[m][n] == enemy || m == 19 || n == 1) && situations[m-1][n+1] == 0)
                            {
                                temp_count = 0;
                                break;
                            }
                            m++;
                            n--;
                        }
                        if(temp_count == 2 && space <= 1)
                        {
                            fake_two++;
                        }
                        temp_count = 0;
                        space = 0;
                    }
                }


                if(true_five > 0)
                {
                    enemy_true[7]++;
                }
                else if(true_four > 0)
                {
                    enemy_true[6]++;
                }
                else if(fake_five > 0)
                {
                    enemy_true[3]++;
                }
                else if(true_three > 0)
                {
                    enemy_true[5]++;
                }
                else if(fake_four > 0)
                {
                    enemy_true[2]++;
                }
                else if(true_two > 0)
                {
                    enemy_true[4]++;
                }
                else if(fake_three > 0)
                {
                    enemy_true[1]++;
                }
                else if(fake_two > 0)
                {
                    enemy_true[0]++;
                }
                true_five = 0,true_four = 0,true_three = 0,true_two = 0;
                fake_two = 0,fake_three = 0,fake_four = 0,fake_five = 0;

enemy_skip_right_oblique:


                swap = enemy;
                enemy = color;
                color = swap;


                situations[i][j] = 0;

                weight[i][j] = friend_true[8] * 15000 + enemy_true[8] * 7450    //六
                             + friend_true[7] * 3700 + enemy_true[7] * 3000     //活五
                             + friend_true[6] * 1200 + enemy_true[6] * 1100      //活四
                             + friend_true[3] * 700 + enemy_true[3] * 600       //冲五
                             + friend_true[5] * 400 + enemy_true[5] * 300       //活三
                             + friend_true[2] * 300 + enemy_true[2] * 200       //冲四
                             + friend_true[4] * 200 + enemy_true[4] * 100       //活二
                             + friend_true[1] * 100 + enemy_true[1] * 50        //冲三
                             + friend_true[0] * 50 + enemy_true[0] * 20;        //冲二

                if( max <= weight[i][j] )
                {
                    if(times % 2 == 1 && judge_rule(i,j) != 0)
                    {
                        continue;
                    }
                    if(max == weight[i][j])
                    {
                        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
                        int rand = qrand() % 10;
                        if(rand < 5)    continue;
                    }
                    max = weight[i][j];
                    max_index = i*100 + j;
//                    test[0] = friend_true[0];
//                    test[1] = friend_true[1];
//                    test[2] = friend_true[2];
//                    test[3] = friend_true[3];
//                    test[4] = friend_true[4];
//                    test[5] = friend_true[5];
//                    test[6] = friend_true[6];
//                    test[7] = friend_true[7];
//                    test[8] = friend_true[8];
//                    test[9] = enemy_true[0];
//                    test[10] = enemy_true[1];
//                    test[11] = enemy_true[2];
//                    test[12] = enemy_true[3];
//                    test[13] = enemy_true[4];
//                    test[14] = enemy_true[5];
//                    test[15] = enemy_true[6];
//                    test[16] = enemy_true[7];
//                    test[17] = enemy_true[8];
                }
            }
        }
    }

    if(max == 0 && times != 1 && times != 0)      //不是第一步，且所有格子权值全部为0时，选取周围空间最大的空格（每个格子周围最多8个空格子）
    {
        int free_space = 1;         //赋值为1，防止此时还出现最大空余空间还是0的情况（即所有剩下空格的四周都没有任何空格）
        int max_free_space = 0;     //初始化最大剩余空间为0
        for (int i = 1;i <= 19;i++)
        {
            for (int j = 1;j <= 19;j++)
            {
                if(situations[i][j] == 0)
                {
                    if(i - 1 > 0 && j - 1 > 0 && situations[i-1][j-1] == 0)      free_space++;
                    if(i + 1 < 20 && j + 1 < 20 && situations[i+1][j+1] == 0)    free_space++;
                    if(i - 1 > 0 && j + 1 < 20 && situations[i-1][j+1] == 0)    free_space++;
                    if(i + 1 < 20 && j - 1 > 0 && situations[i+1][j-1] == 0)    free_space++;
                    if(i + 1 < 20 && situations[i+1][j] == 0)      free_space++;
                    if(i - 1 > 0 && situations[i-1][j] == 0)       free_space++;
                    if(j - 1 > 0 && situations[i][j-1] == 0)       free_space++;
                    if(j + 1 < 20 && situations[i][j+1] == 0)      free_space++;

                    if(max_free_space < free_space)
                    {
                        max_free_space = free_space;
                        max_index = i*100 + j;
                    }

                    free_space = 0;
                }
            }
        }
    }

    situations[max_index/100][max_index%100] = color;


//    qDebug() << "己方：活：" << '\n' << "六 " <<test[8] << " 五 " << test[7] << " 四 " << test[6] << " 三 "<< test[5] << " 二 " << test[4] << '\n';
//    qDebug() << "己方：冲：" << '\n' << "五 " <<test[3] << " 四 " << test[2] << " 三 " << test[1] << " 二 " << test[0] << '\n';

//    qDebug() << "敌方：活：" << '\n' << "六 " <<test[17] << " 五 " << test[16] << " 四 " << test[15] << " 三 "<< test[14] << " 二 " << test[13] << '\n';
//    qDebug() << "敌方：冲：" << '\n' << "五 " <<test[12] << " 四 " << test[11] << " 三 " << test[10] << " 二 " << test[9] << '\n';
//    qDebug() << max;
    return max_index;
}
