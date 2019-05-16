#include <QCoreApplication>

#include "gamepadmonitor.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    GamepadMonitor monitor;
    monitor.init();

    return a.exec();
}
