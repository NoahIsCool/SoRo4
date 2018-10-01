#include "RoverMainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RoverMainWindow w;
    w.show();

    return a.exec();
}
