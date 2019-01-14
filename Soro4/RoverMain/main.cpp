#include <QCoreApplication>

#include "autonomous/autonomous.h"
#include "driveSystem/drivesystem.h"
#include "core/gps/gps.h"

//until we can get the gui working, we will use define flags to determine which system to run
#define DRIVE_SYSTEM

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    /*char *ip = (char*)"192.168.1.123";
    char *host = (char*)"555555";
    gps_init(ip, host);*/

#ifdef DRIVE_SYSTEM
    DriveSystem drive;
#elif AUTONOMOUS
    Autonomous autonomous;
#endif

    gps_finish();
    return a.exec();
}
