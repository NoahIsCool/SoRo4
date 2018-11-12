#include <QCoreApplication>
#include "slavearm.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    SlaveArm* s = new SlaveArm();

    return a.exec();
}
