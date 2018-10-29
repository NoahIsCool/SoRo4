#include <QCoreApplication>

#include "autonomous/autonomous.h"
#include "videoServer/videoserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Autonomous autonomous;
    VideoServer server("test.conf");
    server.run();

    return a.exec();
}
