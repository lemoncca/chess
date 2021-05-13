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

    ui->pushbuttonfind_player->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"       //按钮样式设置
                                             "QPushButton:hover{border-image:url(://image/5.jpg)}");
    ui->pushbuttondisconnect->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"
                                            "QPushButton:hover{border-image:url(://image/5.jpg)}");
    ui->pushButton_order->setStyleSheet("QPushButton{border-image:url(://image/4.jpg);font: 20pt KaiTi;}"
                                        "QPushButton:hover{border-image:url(://image/5.jpg)}");


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
}

void MainWindow::read_data_from_server()
{
    QByteArray buffer = tcpsocket->readAll();
//    qDebug() << buffer;
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
            update();
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
                msg->show();
            }
            else if(buffer[i+3] == 'b')
            {
                //                QMessageBox::information(this,QString("胜负"),QString("对战结束，黑棋赢！\n将自动退出!"));
                msg->setWindowTitle("胜负");
                msg->setText("对战结束，黑棋赢！\n将自动退出!");
                whether_reset_when_msg_close = 1;
                msg->show();
            }
            else if(buffer[i+3] == 'n')
            {
                //                QMessageBox::information(this,QString("胜负"),QString("对战结束，和棋！\n将自动退出!"));
                msg->setWindowTitle("胜负");
                msg->setText("对战结束，和棋！\n将自动退出!");
                whether_reset_when_msg_close = 1;
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
            update();  //调用paintevent，刷新棋盘
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
                msg->show();
            }
            else  if(buffer[i+3] == '5')
            {
                //                QMessageBox::information(this,QString("胜负"),QString("黑棋违反五五禁手，白棋赢！\n对局结束，将自动退出!"));
                msg->setWindowTitle("胜负");
                msg->setText("黑棋违反五五禁手，白棋赢！\n对局结束，将自动退出!");
                whether_reset_when_msg_close = 1;
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
