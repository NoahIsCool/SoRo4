#ifndef SCIENCEPACKAGE_H
#define SCIENCEPACKAGE_H

#include <iostream>

#include "sciencepackage_global.h"
#include "gamepadmonitor.h"

class SCIENCEPACKAGESHARED_EXPORT SciencePackage : public QObject
{

public:
    SciencePackage();
private:
    comms drillComm;
    GamepadMonitor monitor;

public slots:
    void handleMessage(QByteArray arr);
};

#endif // SCIENCEPACKAGE_H
