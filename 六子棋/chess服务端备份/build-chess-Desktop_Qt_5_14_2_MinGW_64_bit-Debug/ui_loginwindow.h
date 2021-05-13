/********************************************************************************
** Form generated from reading UI file 'loginwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWINDOW_H
#define UI_LOGINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_loginwindow
{
public:
    QWidget *centralwidget;
    QWidget *widget;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QWidget *widget_2;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label;
    QPushButton *pushButton_cs;
    QPushButton *pushButton_cc;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *loginwindow)
    {
        if (loginwindow->objectName().isEmpty())
            loginwindow->setObjectName(QString::fromUtf8("loginwindow"));
        loginwindow->resize(1254, 876);
        loginwindow->setMinimumSize(QSize(100, 50));
        centralwidget = new QWidget(loginwindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(120, 590, 900, 150));
        widget->setMinimumSize(QSize(900, 150));
        widget->setMaximumSize(QSize(900, 150));
        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(9, 9, 250, 100));
        pushButton->setMinimumSize(QSize(250, 100));
        pushButton->setMaximumSize(QSize(250, 100));
        pushButton->setStyleSheet(QString::fromUtf8(""));
        pushButton_2 = new QPushButton(widget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(325, 9, 250, 100));
        pushButton_2->setMinimumSize(QSize(250, 100));
        pushButton_2->setMaximumSize(QSize(250, 100));
        pushButton_2->setStyleSheet(QString::fromUtf8(""));
        pushButton_3 = new QPushButton(widget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(641, 9, 250, 100));
        pushButton_3->setMinimumSize(QSize(250, 100));
        pushButton_3->setMaximumSize(QSize(250, 100));
        pushButton_3->setStyleSheet(QString::fromUtf8(""));
        widget_2 = new QWidget(centralwidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setGeometry(QRect(1020, 80, 271, 431));
        gridLayout = new QGridLayout(widget_2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_2 = new QLabel(widget_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setStyleSheet(QString::fromUtf8("font: 70pt \"\345\215\216\346\226\207\350\241\214\346\245\267\";"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        label_3 = new QLabel(widget_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setStyleSheet(QString::fromUtf8("font: 70pt \"\345\215\216\346\226\207\350\241\214\346\245\267\";"));

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        label = new QLabel(widget_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setStyleSheet(QString::fromUtf8("font: 70pt \"\345\215\216\346\226\207\350\241\214\346\245\267\";\n"
""));

        gridLayout->addWidget(label, 2, 0, 1, 1);

        pushButton_cs = new QPushButton(centralwidget);
        pushButton_cs->setObjectName(QString::fromUtf8("pushButton_cs"));
        pushButton_cs->setGeometry(QRect(249, 730, 281, 100));
        pushButton_cs->setMinimumSize(QSize(250, 100));
        pushButton_cs->setMaximumSize(QSize(300, 100));
        pushButton_cs->setStyleSheet(QString::fromUtf8(""));
        pushButton_cc = new QPushButton(centralwidget);
        pushButton_cc->setObjectName(QString::fromUtf8("pushButton_cc"));
        pushButton_cc->setGeometry(QRect(610, 730, 281, 100));
        pushButton_cc->setMinimumSize(QSize(250, 100));
        pushButton_cc->setMaximumSize(QSize(300, 100));
        pushButton_cc->setStyleSheet(QString::fromUtf8(""));
        loginwindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(loginwindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1254, 21));
        loginwindow->setMenuBar(menubar);
        statusbar = new QStatusBar(loginwindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        loginwindow->setStatusBar(statusbar);

        retranslateUi(loginwindow);

        QMetaObject::connectSlotsByName(loginwindow);
    } // setupUi

    void retranslateUi(QMainWindow *loginwindow)
    {
        loginwindow->setWindowTitle(QCoreApplication::translate("loginwindow", "MainWindow", nullptr));
        pushButton->setText(QCoreApplication::translate("loginwindow", "\344\272\272\346\234\272\345\257\271\346\210\230", nullptr));
        pushButton_2->setText(QCoreApplication::translate("loginwindow", "\346\234\254\345\234\260\345\217\214\344\272\272\345\257\271\346\210\230", nullptr));
        pushButton_3->setText(QCoreApplication::translate("loginwindow", "\347\224\265\350\204\221\345\257\271\346\210\230", nullptr));
        label_2->setText(QCoreApplication::translate("loginwindow", "\345\255\220", nullptr));
        label_3->setText(QCoreApplication::translate("loginwindow", "\346\243\213", nullptr));
        label->setText(QCoreApplication::translate("loginwindow", "\345\205\255", nullptr));
        pushButton_cs->setText(QCoreApplication::translate("loginwindow", "\346\234\215\345\212\241\347\253\257\345\257\271\346\210\230\345\256\242\346\210\267\347\253\257", nullptr));
        pushButton_cc->setText(QCoreApplication::translate("loginwindow", "\345\256\242\346\210\267\347\253\257\345\257\271\346\210\230\345\256\242\346\210\267\347\253\257", nullptr));
    } // retranslateUi

};

namespace Ui {
    class loginwindow: public Ui_loginwindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
