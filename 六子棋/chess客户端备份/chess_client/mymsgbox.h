#ifndef MYMSGBOX_H
#define MYMSGBOX_H

#include <QWidget>
#include <QMessageBox>

class mymsgbox : public QMessageBox
{
    Q_OBJECT
public:
    explicit mymsgbox(QWidget *parent = nullptr);

    void closeEvent(QCloseEvent *event) override;
signals:
    void msg_is_closed();
};

#endif // MYMSGBOX_H
