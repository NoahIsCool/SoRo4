#include <QCoreApplication>

#include "autonomous/autonomous.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Autonomous autonomous;

    return a.exec();
}
