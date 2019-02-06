#ifndef AUTONOMOUSTESTER_H
#define AUTONOMOUSTESTER_H

#include "../autonomous.h"


class autonomousTester : public autonomous
{
    public:
        autonomousTester();

    private:
        int testGetWheelSpeedValues();
        int testGetAngleToTurn();
        int testInputNextCords();
        int testUpdateStatus();
};

#endif // AUTONOMOUSTESTER_H
