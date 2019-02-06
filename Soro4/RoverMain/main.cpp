#include <QCoreApplication>

#include "autonomous/autonomous.h"
#include "driveSystem/drivesystem.h"
#include "core/gps/gps.h"

//until we can get the gui working, we will use define flags to determine which system to run
//#define DRIVE_SYSTEM
#define AUTONOMOUS

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    char *ip = (char*)"192.168.1.222";
    char *host = (char*)"55556";
    gps_init(ip, host);

//#ifdef DRIVE_SYSTEM
//    DriveSystem driveSystem;
//#elif AUTONOMOUS
    Autonomous autonomous;
//#endif

    gps_finish();
    return a.exec();
}
