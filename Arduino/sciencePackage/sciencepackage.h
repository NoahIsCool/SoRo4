#ifndef SCIENCEPACKAGE_H
#define SCIENCEPACKAGE_H

#include <QObject>
#include <iostream>
#include <fstream>

#include "sciencepackage_global.h"
#include "gamepadmonitor.h"

class SciencePackage : public QObject
{
    Q_OBJECT
public:
    explicit SciencePackage(QObject *parent = nullptr);
    ~SciencePackage();

signals:

public slots:
    void handleMessage(QByteArray message);

private:
    comms drillComm;
    GamepadMonitor monitor;
    std::fstream csvFile;
};

#endif // SCIENCEPACKAGE_H
