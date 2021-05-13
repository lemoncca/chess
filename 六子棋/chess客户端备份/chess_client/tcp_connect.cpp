#include "tcp_connect.h"
#include "ui_tcp_connect.h"

tcp_connect::tcp_connect(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tcp_connect)
{
    ui->setupUi(this);

    connect(ui->pushButton_connect,&QPushButton::clicked,this,&tcp_connect::get_ip_and_port);
    connect(ui->pushButton_connect,&QPushButton::pressed,this,&tcp_connect::set_button_text_wait);
}

tcp_connect::~tcp_connect()
{
    delete ui;
}

void tcp_connect::get_ip_and_port()
{
    emit get_IP_Port();
}

QString tcp_connect::get_ip()
{
    QString ip = ui->lineEdit_IP->text();
    return ip;
}

quint16 tcp_connect::get_port()
{
        quint16 port = ui->lineEdit_port->text().toUInt();
        return port;
}

void tcp_connect::set_button_text_wait()
{
    ui->pushButton_connect->setText("等待连接");
}

void tcp_connect::set_button_text_connect()
{
    ui->pushButton_connect->setText("连接");
}


