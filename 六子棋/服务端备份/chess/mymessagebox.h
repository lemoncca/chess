#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <QWidget>
#include <QMessageBox>

class MymessageBox : public QMessageBox
{
    Q_OBJECT
public:
    explicit MymessageBox(QWidget *parent = nullptr);

    void closeEvent(QCloseEvent *event) override;

signals:
    void msg_is_closed();

};

#endif // MYMESSAGEBOX_H
