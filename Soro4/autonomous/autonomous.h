#ifndef AUTONOMOUS_H
#define AUTONOMOUS_H

#include <QDebug>
#include <QObject>

#include "autonomous_global.h"
#include "SearchAlgorithm.h"
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

class AUTONOMOUSSHARED_EXPORT Autonomous : public QObject
{
    public:
        Autonomous();

    private:
        void mainLoop();
        std::vector<double> getWheelSpeedValues(double amountOff, double baseSpeed);
         //FIXME: was std::vector. Should it be a list or a vector?
        std::list<Cell> GeneratePath(Cell dest);
        void avoidObstacle();
        double getAngleToTurn(Cell next);
        Cell inputNextCoords();
        void updateStatus();

        bool isThereObstacle;
        double speed = 60; //IDK what we want for speed right now or if we want to be updating it but this currently represents the speed that we want the rover to be going at for everything
        volatile double angle; //Updated through updateStatus
        double lastLongitude = 0; //the last longitude updated from updateStatus
        double lastLatitude = 0; //the last latitude update from updateStatus
        bool threadsRunning = true; //checks if the threads should be running (currently just stops updateStatus
        comms mySocket;
        int timesStuck; // finds how many times the rover has been stuck in place
        bool isStuck; //checks if the rover is considered stuck
        SearchAlgorithm searcher;

        double centerLidarMaxHeight = 10; //the max height that the center lidar can find before it declares an obstacle
        double middleLidarMaxHeight = 10; //the max height that the middle lidars can find before it declares an obstacle
        double outerLidarMaxHeight = 10; //the max height that the outer lidars can find before it declares an obstacle
        const double lidarAngle = 10; //the angle of the Lidars to the rover
        const double lidarHeights = 10; //the height of the lidars from the ground


        //outerLeftLidar, innerLeftLidar, centerLidar, innerRightLidar, outerRightLidar
        int *obstacleDistancess = new int[5];//holds the readings from the Lidars
        int *maxObstacleHeights = {10, 10, 10, 10, 10}; //The distances the Lidars see if there is an obstacle TODO: get actual values for this

        //innerLeftLidar, centerLidar, innerRightLidar
        int *maxHoleDepths = {20, 20, 20}; //The distances the Lidars see if there is a hole TODO: get actual values for this

    private slots:
        void lidarValues(QByteArray message);
};

#endif // AUTONOMOUS_H
