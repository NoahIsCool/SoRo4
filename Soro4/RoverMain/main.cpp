#include <QCoreApplication>

#include "autonomous/autonomous.h"
#include "core/gps/gps.h"
#include "autonomous/cachetracking.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    char *ip = (char*)"192.0.0.123";
    char *host = (char*)"55555";
    //gps_init(ip, host);

    //Autonomous autonomous;
    CacheTracking tracker("/dev/video1");

    //gps_finish();
    return a.exec();
}
