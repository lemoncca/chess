/********************************************************************************
** Form generated from reading UI file 'tcp_connect.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCP_CONNECT_H
#define UI_TCP_CONNECT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_tcp_connect
{
public:
    QWidget *centralwidget;
    QLineEdit *lineEdit_port;
    QLabel *label_port;
    QLabel *label_IP;
    QLineEdit *lineEdit_IP;
    QPushButton *pushButton_connect;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *tcp_connect)
    {
        if (tcp_connect->objectName().isEmpty())
            tcp_connect->setObjectName(QString::fromUtf8("tcp_connect"));
        tcp_connect->resize(500, 280);
        tcp_connect->setMinimumSize(QSize(500, 280));
        tcp_connect->setMaximumSize(QSize(500, 280));
        centralwidget = new QWidget(tcp_connect);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        lineEdit_port = new QLineEdit(centralwidget);
        lineEdit_port->setObjectName(QString::fromUtf8("lineEdit_port"));
        lineEdit_port->setGeometry(QRect(140, 40, 300, 45));
        lineEdit_port->setMinimumSize(QSize(300, 45));
        lineEdit_port->setMaximumSize(QSize(300, 45));
        QFont font;
        font.setFamily(QString::fromUtf8("Agency FB"));
        font.setPointSize(11);
        lineEdit_port->setFont(font);
        lineEdit_port->setStyleSheet(QString::fromUtf8("QLineEdit{qproperty-alignment:AlignHCenter;}\n"
"font: 11pt \"\346\226\260\345\256\213\344\275\223\";"));
        label_port = new QLabel(centralwidget);
        label_port->setObjectName(QString::fromUtf8("label_port"));
        label_port->setGeometry(QRect(23, 39, 110, 45));
        label_port->setMinimumSize(QSize(110, 45));
        label_port->setMaximumSize(QSize(110, 45));
        label_IP = new QLabel(centralwidget);
        label_IP->setObjectName(QString::fromUtf8("label_IP"));
        label_IP->setGeometry(QRect(23, 99, 110, 45));
        label_IP->setMinimumSize(QSize(110, 45));
        label_IP->setMaximumSize(QSize(110, 45));
        lineEdit_IP = new QLineEdit(centralwidget);
        lineEdit_IP->setObjectName(QString::fromUtf8("lineEdit_IP"));
        lineEdit_IP->setGeometry(QRect(140, 100, 300, 45));
        lineEdit_IP->setMinimumSize(QSize(300, 45));
        lineEdit_IP->setMaximumSize(QSize(300, 45));
        lineEdit_IP->setFont(font);
        lineEdit_IP->setStyleSheet(QString::fromUtf8("QLineEdit{qproperty-alignment:AlignHCenter;}\n"
"font: 11pt \"\346\226\260\345\256\213\344\275\223\";"));
        pushButton_connect = new QPushButton(centralwidget);
        pushButton_connect->setObjectName(QString::fromUtf8("pushButton_connect"));
        pushButton_connect->setGeometry(QRect(150, 170, 200, 50));
        pushButton_connect->setMinimumSize(QSize(200, 50));
        pushButton_connect->setMaximumSize(QSize(200, 50));
        tcp_connect->setCentralWidget(centralwidget);
        menubar = new QMenuBar(tcp_connect);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 500, 21));
        tcp_connect->setMenuBar(menubar);
        statusbar = new QStatusBar(tcp_connect);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        tcp_connect->setStatusBar(statusbar);

        retranslateUi(tcp_connect);

        QMetaObject::connectSlotsByName(tcp_connect);
    } // setupUi

    void retranslateUi(QMainWindow *tcp_connect)
    {
        tcp_connect->setWindowTitle(QCoreApplication::translate("tcp_connect", "MainWindow", nullptr));
        lineEdit_port->setText(QCoreApplication::translate("tcp_connect", "88", nullptr));
        label_port->setText(QCoreApplication::translate("tcp_connect", "\346\234\215\345\212\241\345\231\250\347\253\257\345\217\243\357\274\232", nullptr));
        label_IP->setText(QCoreApplication::translate("tcp_connect", "\346\234\215\345\212\241\345\231\250IP\357\274\232", nullptr));
        lineEdit_IP->setText(QCoreApplication::translate("tcp_connect", "127.0.0.1", nullptr));
        pushButton_connect->setText(QCoreApplication::translate("tcp_connect", "\350\277\236\346\216\245", nullptr));
    } // retranslateUi

};

namespace Ui {
    class tcp_connect: public Ui_tcp_connect {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCP_CONNECT_H
