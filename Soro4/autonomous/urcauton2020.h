#ifndef URCAUTON2020_H
#define URCAUTON2020_H

/*
 * This is the main autonomous program for the 2020 urc event
 *
 * This code will attempt to do the following:
 *   1. Take in a list of coordinates that the robot will travel to
 *   2. Generate a path to the first set of coordinates using AStar
 *   3. Travel along the path while looking for a tennis ball
 *      3a. Will use a GPS to follow the path exactly
 *      3b. Will use a gyro and possibly accel to find the orientation of robot
 *      3c. Will use LIDAR sensor to find and avoid large obsticles
 *   4. Track the tennis ball using a camera and OpenCV
 *   5. Note that it found the tennis ball and stop and wait
 *   6. Repeat for all other sets of coordinates
 *
 * NOTE: we will also want to have some sort of GUI to track progress for debug and sanity
 */

#include <QDebug>
#include <QObject>

#include "autonomous_global.h"
//#include "SearchAlgorithm.h"
#include "core/core.h"
#include "core/comms.h"
#include "core/gps/gps.h"

#include <list>
#include <math.h>
#include <thread>
#include <iostream>
#include <queue>
#include <set>
#include <unistd.h>

class URCAuton2020
{
    public:
        URCAuton2020();

    private:
        void mainLoop();

        std::vector<double> getWheelSpeedValues(double amountOff, double baseSpeed);
        void testGetWheelSpeedValues();
};

#endif // URCAUTON2020_H
