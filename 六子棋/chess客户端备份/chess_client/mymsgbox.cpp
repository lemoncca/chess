#include "mymsgbox.h"

mymsgbox::mymsgbox(QWidget *parent) : QMessageBox(parent)
{

}

void  mymsgbox::closeEvent(QCloseEvent *)
{
    emit msg_is_closed();
}
