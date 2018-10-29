#include <QCoreApplication>

#include "autonomous/autonomous.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    char *ip = "192.168.1.123";
    char *host = "55555";
    gps_init(ip, host);

    Autonomous autonomous;

    return a.exec();
}
