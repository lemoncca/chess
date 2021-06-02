/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *pushbuttonfind_player;
    QPushButton *pushbuttondisconnect;
    QPushButton *pushButton_order;
    QPushButton *pushButton_pattern2;
    QPushButton *pushButton_pattern1;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1492, 1076);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        pushbuttonfind_player = new QPushButton(centralwidget);
        pushbuttonfind_player->setObjectName(QString::fromUtf8("pushbuttonfind_player"));
        pushbuttonfind_player->setGeometry(QRect(950, 140, 271, 131));
        pushbuttondisconnect = new QPushButton(centralwidget);
        pushbuttondisconnect->setObjectName(QString::fromUtf8("pushbuttondisconnect"));
        pushbuttondisconnect->setGeometry(QRect(950, 620, 271, 131));
        pushButton_order = new QPushButton(centralwidget);
        pushButton_order->setObjectName(QString::fromUtf8("pushButton_order"));
        pushButton_order->setGeometry(QRect(950, 380, 271, 141));
        pushButton_pattern2 = new QPushButton(centralwidget);
        pushButton_pattern2->setObjectName(QString::fromUtf8("pushButton_pattern2"));
        pushButton_pattern2->setGeometry(QRect(890, 150, 181, 131));
        pushButton_pattern1 = new QPushButton(centralwidget);
        pushButton_pattern1->setObjectName(QString::fromUtf8("pushButton_pattern1"));
        pushButton_pattern1->setGeometry(QRect(1100, 150, 181, 131));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1492, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushbuttonfind_player->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\345\257\273\346\211\276\345\257\271\346\211\213", nullptr));
        pushbuttondisconnect->setText(QCoreApplication::translate("MainWindow", "\346\226\255\345\274\200\350\277\236\346\216\245", nullptr));
        pushButton_order->setText(QCoreApplication::translate("MainWindow", "\347\255\211\345\276\205\346\212\275\345\217\226\351\241\272\345\272\217", nullptr));
        pushButton_pattern2->setText(QCoreApplication::translate("MainWindow", "AI\344\273\243\347\220\206", nullptr));
        pushButton_pattern1->setText(QCoreApplication::translate("MainWindow", "\346\211\213\345\212\250\344\270\213\346\243\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
