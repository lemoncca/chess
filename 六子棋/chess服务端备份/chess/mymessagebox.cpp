#include "mymessagebox.h"

MymessageBox::MymessageBox(QWidget *parent) : QMessageBox(parent)
{

}

void MymessageBox::closeEvent(QCloseEvent *)
{
    emit msg_is_closed();
}
