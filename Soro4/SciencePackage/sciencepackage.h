#ifndef SCIENCEPACKAGE_H
#define SCIENCEPACKAGE_H

#include "sciencepackage_global.h"

#include <QObject>

class SCIENCEPACKAGESHARED_EXPORT SciencePackage : public QObject
{

public:
    SciencePackage();

private:
    void mainLoop();

private slots:
    void recieve(QByteArray message);
};

#endif // SCIENCEPACKAGE_H
