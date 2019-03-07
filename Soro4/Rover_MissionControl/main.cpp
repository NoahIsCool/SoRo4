#include <QCoreApplication>
#include <signal.h>

#include "driveController/gamepadmonitor.h"

//until we can get the gui working, we will use define flags to determine which system to run

void my_handler(int s){
           qDebug() << "killing mission control";
#ifdef SCIENCE_PACKAGE
            delete package;
#endif
           exit(1);

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

#ifdef DRIVE_SYSTEM
    GamepadMonitor drivepad;
#endif

    return a.exec();
}
