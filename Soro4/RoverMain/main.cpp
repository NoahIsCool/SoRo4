#include <QCoreApplication>

#include "autonomous/autonomous.h"
#include "core/gps/gps.h"
#include "autonomous/cachetracking.h"
#include "autonomous/erctraversal.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //char *ip = (char*)"192.168.1.222";
    //char *host = (char*)"55556";
    //gps_init(ip, host);

    //Autonomous autonomous;
    //CacheTracking tracker("/dev/video1");

    ERCTraversal test("/dev/video0");

    //gps_finish();
    return a.exec();
}
