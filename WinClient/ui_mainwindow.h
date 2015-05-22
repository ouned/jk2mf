/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *txtPath;
    QPushButton *btnPath;
    QPushButton *btnInstall;
    QPushButton *btnQuit;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(557, 313);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(557, 313));
        MainWindow->setMaximumSize(QSize(557, 313));
        QIcon icon;
        icon.addFile(QStringLiteral(":/logo/jk2mf.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(190, 10, 160, 160));
        label->setPixmap(QPixmap(QString::fromUtf8(":/logo/jk2mf.png")));
        label->setScaledContents(true);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 30, 61, 16));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 10, 91, 16));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(510, 6, 41, 20));
        label_4->setOpenExternalLinks(true);
        label_4->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 210, 46, 13));
        txtPath = new QLineEdit(centralWidget);
        txtPath->setObjectName(QStringLiteral("txtPath"));
        txtPath->setGeometry(QRect(10, 230, 491, 20));
        btnPath = new QPushButton(centralWidget);
        btnPath->setObjectName(QStringLiteral("btnPath"));
        btnPath->setGeometry(QRect(510, 230, 31, 20));
        btnInstall = new QPushButton(centralWidget);
        btnInstall->setObjectName(QStringLiteral("btnInstall"));
        btnInstall->setEnabled(false);
        btnInstall->setGeometry(QRect(448, 280, 101, 25));
        btnQuit = new QPushButton(centralWidget);
        btnQuit->setObjectName(QStringLiteral("btnQuit"));
        btnQuit->setGeometry(QRect(10, 280, 101, 25));
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "JK2MF Clientside Installation", 0));
        label->setText(QString());
        label_2->setText(QApplication::translate("MainWindow", "<html><head/><body><p>Version: 3.0</p></body></html>", 0));
        label_3->setText(QApplication::translate("MainWindow", "JK2MF Clientside", 0));
        label_4->setText(QApplication::translate("MainWindow", "<a href=\"http://jk2.ouned.de/jk2mf\">Website<a/>", 0));
        label_5->setText(QApplication::translate("MainWindow", "JK2 Path:", 0));
        btnPath->setText(QApplication::translate("MainWindow", "...", 0));
        btnInstall->setText(QApplication::translate("MainWindow", "Install", 0));
        btnQuit->setText(QApplication::translate("MainWindow", "Quit", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
