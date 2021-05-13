#include <mainwindow.h>


int MainWindow::judge_rule(int i, int j)   //禁手判断
{
    int a = i,b = j;
    int ban_four = 0,ban_five = 0;      //记录某点有几个活五、活四
    int space = 0;          //空格数
    int temp_count = 0;     //暂时计数器

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



bool MainWindow::judge_win(int i, int j)    //这里尝试了另一种对某点的棋子计算方法(向左遍历找到最后一个相连的友方，向右同理)
{
    if(times % 2 == 1)   //黑棋判断
    {
        int a = i;
        int b = j;
        int sum[4] = {0};


        while(a > 0 && situations[a][j] == 1)   //水平方向
        {
            sum[0]++;
            a--;
        }
        a = i + 1;
        while(a < 20 && situations[a][j] == 1)
        {
            sum[0]++;
            a++;
        }
        a = i;
        if(sum[0] >= 6) goto judge_black;


        while(b > 0 && situations[i][b] == 1)  //竖直方向
        {
            sum[1]++;
            b--;
        }
        b = j + 1;
        while(b < 20 && situations[i][b] == 1)
        {
            sum[1]++;
            b++;
        }
        b = j;
        if(sum[1] >= 6) goto judge_black;


        while(b > 0 && a > 0 && situations[a][b] == 1)  //左上右下
        {
            sum[2]++;
            b--;
            a--;
        }
        b = j + 1;
        a = i + 1;
        while(b < 20 && a < 20 && situations[a][b] == 1)
        {
            sum[2]++;
            b++;
            a++;
        }
        b = j;
        a = i;
        if(sum[2] >= 6) goto judge_black;


        while(b > 0 && a < 20 && situations[a][b] == 1)  //左下右上
        {
            sum[3]++;
            b--;
            a++;
        }
        b = j + 1;
        a = i - 1;
        while(b < 20 && a > 0 && situations[a][b] == 1)
        {
            sum[3]++;
            b++;
            a--;
        }
        b = j;
        a = i;
        if(sum[3] >= 6) goto judge_black;

judge_black:
        if( sum[0] >= 6 || sum[1] >= 6 || sum[2] >= 6 || sum[3] >= 6 )
        {
            if((pattern == 4 && is_listen && client_num >= 1) || (pattern == 5 && is_listen && client_num >= 2))
            {
                QString str = "winb";
                if(pattern == 4)
                {
                    socket = tcpsocket_list[0];
                    socket->write(str.toUtf8().data());
                }
                else if(pattern == 5)
                {
                    foreach(QTcpSocket *i,tcpsocket_list)
                    {
                        socket = i;
                        socket->write(str.toUtf8().data());
                    }
                }
            }

            //            QMessageBox *win_box = new QMessageBox(QMessageBox::Information,QString("黑棋赢！"),QString("重新开始"));
            //            QTimer::singleShot(2000,win_box,SLOT(accept() ) );
            //            QApplication::setQuitOnLastWindowClosed(false);
            //            win_box->exec();

            msg->setWindowTitle("胜负");
            msg->setText("黑棋赢！");
            whether_reset_when_msg_close = 1;
            if(pattern == 4 || pattern == 5)
            {
                client_num = 0;
                update();
            }
            msg->show();

            return 1;
        }
        else if(times == 361)   //和棋情况，由于棋子数是奇数，所以最后一个棋子是黑棋下的，即第361个棋
        {                       //若第361个棋黑棋还没有赢，那么和棋
            if((pattern == 4 && is_listen && client_num >= 1) || (pattern == 5 && is_listen && client_num >= 2))
            {
                QString str = "winn";
                if(pattern == 4)
                {
                    socket = tcpsocket_list[0];
                    socket->write(str.toUtf8().data());
                }
                else if(pattern == 5)
                {
                    foreach(QTcpSocket *i,tcpsocket_list)
                    {
                        socket = i;
                        socket->write(str.toUtf8().data());
                    }
                }
            }

            //            QMessageBox *win_box = new QMessageBox(QMessageBox::Information,QString("和棋！"),QString("重新开始"));
            //            QTimer::singleShot(2000,win_box,SLOT(accept() ) );
            //            QApplication::setQuitOnLastWindowClosed(false);
            //            win_box->exec();

            msg->setWindowTitle("胜负");
            msg->setText("和棋");
            whether_reset_when_msg_close = 1;
            if(pattern == 4 || pattern == 5)
            {
                client_num = 0;
                update();
            }
            msg->show();

            return 1;
        }
        return 0;
    }


    else if(times % 2 == 0)   //与黑棋相同，但是没有和棋情况
    {
        int a = i;
        int b = j;
        int sum[4] = {0};

        while(a > 0 && situations[a][j] == 2)
        {
            sum[0]++;
            a--;
        }
        a = i + 1;
        while(a < 20 && situations[a][j] == 2)
        {
            sum[0]++;
            a++;
        }
        a = i;
        if(sum[0] >= 6) goto judge_white;

        while(b > 0 && situations[i][b] == 2)
        {
            sum[1]++;
            b--;
        }
        b = j + 1;
        while(b < 20 && situations[i][b] == 2)
        {
            sum[1]++;
            b++;
        }
        b = j;
        if(sum[1] >= 6) goto judge_white;

        while(b > 0 && a > 0 && situations[a][b] == 2)
        {
            sum[2]++;
            b--;
            a--;
        }
        b = j + 1;
        a = i + 1;
        while(b < 20 && a < 20 && situations[a][b] == 2)
        {
            sum[2]++;
            b++;
            a++;
        }
        b = j;
        a = i;
        if(sum[2] >= 6) goto judge_white;

        while(b > 0 && a < 20 && situations[a][b] == 2)
        {
            sum[3]++;
            b--;
            a++;
        }
        b = j + 1;
        a = i - 1;
        while(b < 20 && a > 0 && situations[a][b] == 2)
        {
            sum[3]++;
            b++;
            a--;
        }
        b = j;
        a = i;
        if(sum[3] >= 6) goto judge_white;

judge_white:
        if( sum[0] >= 6 || sum[1] >= 6 || sum[2] >= 6 || sum[3] >= 6 )
        {
            if((pattern == 4 && is_listen && client_num >= 1) || (pattern == 5 && is_listen && client_num >= 2))
            {
                QString str = "winw";
                if(pattern == 4)
                {
                    socket = tcpsocket_list[0];
                    socket->write(str.toUtf8().data());
                }
                else if(pattern == 5)
                {
                    foreach(QTcpSocket *i,tcpsocket_list)
                    {
                        socket = i;
                        socket->write(str.toUtf8().data());
                    }
                }
            }

            //            QMessageBox *win_box = new QMessageBox(QMessageBox::Information,QString("白棋赢！"),QString("重新开始"));
            //            QTimer::singleShot(2000,win_box,SLOT(accept() ) );
            //            QApplication::setQuitOnLastWindowClosed(false);
            //            win_box->exec();

            msg->setWindowTitle("胜负");
            msg->setText("白棋赢！");
            whether_reset_when_msg_close = 1;
            if(pattern == 4 || pattern == 5)
            {
                client_num = 0;
                update();
            }
            msg->show();

            return 1;
        }
        return 0;
    }

    return 0;
}
