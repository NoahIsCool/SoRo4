#include <QCoreApplication>

#include "videoServer/videoserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    VideoServer server("/opt/project-remoteVideo/config/server.conf");

    server.run();

    return a.exec();
}
