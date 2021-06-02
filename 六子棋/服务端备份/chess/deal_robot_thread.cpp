#include <mainwindow.h>


void MainWindow::deal_robot_final_done()
{
    msg->setWindowTitle("robot_thread");
    msg->setText("AI 线程已经关闭！");
    msg->show();
}

void MainWindow::stop_thread()
{
        Robot_thread->quit();
        Robot_thread->wait();
}

void MainWindow::deal_robot_done(int x,int y)
{
    update();
    judge_win(x,y);
}
