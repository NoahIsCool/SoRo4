/********************************************************************************
** Form generated from reading UI file 'drivecontroller.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DRIVECONTROLLER_H
#define UI_DRIVECONTROLLER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DriveController
{
public:
    QPushButton *resetButton;
    QListWidget *consoleList;

    void setupUi(QWidget *DriveController)
    {
        if (DriveController->objectName().isEmpty())
            DriveController->setObjectName(QLatin1String("DriveController"));
        DriveController->resize(400, 300);
        resetButton = new QPushButton(DriveController);
        resetButton->setObjectName(QLatin1String("resetButton"));
        resetButton->setGeometry(QRect(10, 270, 89, 25));
        consoleList = new QListWidget(DriveController);
        consoleList->setObjectName(QLatin1String("consoleList"));
        consoleList->setGeometry(QRect(10, 10, 381, 251));

        retranslateUi(DriveController);

        QMetaObject::connectSlotsByName(DriveController);
    } // setupUi

    void retranslateUi(QWidget *DriveController)
    {
        DriveController->setWindowTitle(QApplication::translate("DriveController", "Form", nullptr));
        resetButton->setText(QApplication::translate("DriveController", "Reset", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DriveController: public Ui_DriveController {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DRIVECONTROLLER_H
