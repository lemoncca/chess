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
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1325, 956);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        pushButton_4 = new QPushButton(centralwidget);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(960, 650, 251, 131));
        pushButton_4->setStyleSheet(QString::fromUtf8("font: 22pt \"\345\215\216\346\226\207\346\245\267\344\275\223\";\n"
"border-image: url(:/image/4.jpg);"));
        pushButton_5 = new QPushButton(centralwidget);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setGeometry(QRect(960, 140, 251, 131));
        pushButton_5->setStyleSheet(QString::fromUtf8("font: 22pt \"\345\215\216\346\226\207\346\245\267\344\275\223\";\n"
"border-image: url(:/image/4.jpg);"));
        pushButton_6 = new QPushButton(centralwidget);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setGeometry(QRect(950, 140, 121, 131));
        pushButton_6->setStyleSheet(QString::fromUtf8("font: 22pt \"\345\215\216\346\226\207\346\245\267\344\275\223\";\n"
"border-image: url(:/image/4.jpg);"));
        pushButton_7 = new QPushButton(centralwidget);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        pushButton_7->setGeometry(QRect(1100, 140, 121, 131));
        pushButton_7->setStyleSheet(QString::fromUtf8("font: 22pt \"\345\215\216\346\226\207\346\245\267\344\275\223\";\n"
"border-image: url(:/image/4.jpg);"));
        pushButton_8 = new QPushButton(centralwidget);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setGeometry(QRect(960, 140, 251, 131));
        pushButton_8->setStyleSheet(QString::fromUtf8("font: 22pt \"\345\215\216\346\226\207\346\245\267\344\275\223\";\n"
"border-image: url(:/image/4.jpg);"));
        pushButton_9 = new QPushButton(centralwidget);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        pushButton_9->setGeometry(QRect(960, 390, 251, 131));
        pushButton_9->setStyleSheet(QString::fromUtf8("font: 22pt \"\345\215\216\346\226\207\346\245\267\344\275\223\";\n"
"border-image: url(:/image/4.jpg);"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1325, 21));
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
        pushButton_4->setText(QCoreApplication::translate("MainWindow", "\350\277\224\345\233\236\344\270\273\350\217\234\345\215\225", nullptr));
        pushButton_5->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\346\270\270\346\210\217", nullptr));
        pushButton_6->setText(QCoreApplication::translate("MainWindow", "\345\205\210\346\211\213", nullptr));
        pushButton_7->setText(QCoreApplication::translate("MainWindow", "\345\220\216\346\211\213", nullptr));
        pushButton_8->setText(QCoreApplication::translate("MainWindow", "\347\224\265\350\204\221\357\274\232\344\270\213\344\270\200\346\255\245", nullptr));
        pushButton_9->setText(QCoreApplication::translate("MainWindow", "\351\207\215\346\226\260\345\274\200\345\247\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
