#ifndef ERCTRAVERSAL_H
#define ERCTRAVERSAL_H
#include <vector>
#include <unistd.h>
#include <iostream>
#include <math.h>
#include <thread>

#include "core/core.h"
#include "core/comms.h"
#include "core/gps/imu.h"

class ERCTraversal
{
    public:
        ERCTraversal();
    private:
        double currentAngle = 0;
        uint16_t biasZ;
        msg_imu_raw_t gyro;

        std::vector<double> getWheelSpeeds(double angleOff, double baseSpeed);
        void updateAngle();
        int testMain();
        //add camera thread method
        //add lidar stuff
        //add GUI stuff
        //create a main loop that allows for us to very easily be able to change it to fit their map
};

#endif
