#include <QCoreApplication>

#include "driveController/gamepadmonitor.h"
#include "sciencePackage/sciencepackage.h"

//until we can get the gui working, we will use define flags to determine which system to run
#define DRIVE_SYSTEM

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef DRIVE_SYSTEM
    GamepadMonitor drivepad;
#endif

#ifdef SCIENCE_PACKAGE
    SciencePackage package;
#endif

    return a.exec();
}
