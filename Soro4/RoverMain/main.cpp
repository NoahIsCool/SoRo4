﻿#include <QCoreApplication>

#include "autonomous/autonomous.h"
#include "core/gps/gps.h"
#include "VideoServer/videoserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    /*char *ip = (char*)"192.168.1.123";
    char *host = (char*)"555555";
    gps_init(ip, host);

    Autonomous autonomous;

    gps_finish();*/

    VideoServer server("/opt/project-remoteVideo/config/server.conf");
    server.run();

    return a.exec();
}
