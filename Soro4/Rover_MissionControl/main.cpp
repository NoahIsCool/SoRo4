#include <QApplication>
#include "../driveController/drivecontroller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DriveController controller;
    controller.showWindow();

    return a.exec();
}
