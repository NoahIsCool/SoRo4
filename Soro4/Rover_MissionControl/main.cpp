﻿#include <QCoreApplication>

#include "driveController/gamepadmonitor.h"

//until we can get the gui working, we will use define flags to determine which system to run
#define DRIVE_SYSTEM

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef DRIVE_SYSTEM
    GamepadMonitor drivepad;
#endif

    return a.exec();
}
