#ifndef TCP_CONNECT_H
#define TCP_CONNECT_H

#include <QMainWindow>
#include <QTcpSocket>

namespace Ui {
class tcp_connect;
}

class tcp_connect : public QMainWindow
{
    Q_OBJECT

public:
    explicit tcp_connect(QWidget *parent = nullptr);
    ~tcp_connect();

    QString get_ip();
    quint16 get_port();

    void set_button_text_wait();
    void set_button_text_connect();

private:
    Ui::tcp_connect *ui;

signals:
    void get_IP_Port();

public slots:
    void get_ip_and_port();
};

#endif // TCP_CONNECT_H
