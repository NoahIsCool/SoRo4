#ifndef DRIVECONTROLLER_H
#define DRIVECONTROLLER_H

#include "drivecontroller_global.h"
#include "drivewindow.h"
#include <QObject>

class DRIVECONTROLLERSHARED_EXPORT DriveController
{
private:
    // lmao what are coding conventions???
    driveWindow window;
public:
    DriveController();
    void showWindow();
};

#endif // DRIVECONTROLLER_H
