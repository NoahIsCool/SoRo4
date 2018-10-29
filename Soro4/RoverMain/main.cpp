#include <QCoreApplication>

#include "autonomous/autonomous.h"
#include "videoServer/videoserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    char *ip = "192.168.1.123";
    char *host = "55555";
    gps_init(ip, host);

    Autonomous autonomous;
    VideoServer server("test.conf");
    server.run();

    return a.exec();
}
