#ifndef DRIVECONTROLLER_H
#define DRIVECONTROLLER_H

#include "drivecontroller_global.h"
#include "drivewindow.h"

class DRIVECONTROLLERSHARED_EXPORT DriveController
{
private:
    driveWindow window;
public:
    DriveController();
    void showWindow();
};

#endif // DRIVECONTROLLER_H
