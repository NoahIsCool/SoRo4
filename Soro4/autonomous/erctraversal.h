#ifndef ERCTRAVERSAL_H
#define ERCTRAVERSAL_H
#include <vector>
#include <unistd.h>
#include <iostream>
#include <math.h>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <aruco/markerdetector.h>
#include <opencv2/flann.hpp>
#include <opencv2/aruco.hpp>

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
        int baseSpeed = 60; //this will likely need adjusted
        comms mySocket;
        int stopArea = 1000; //this will be the area that the aruco marker is when we want to stop. TODO: Adjust this value
        int currentArea; //current size of the marker
        int arucoCenter = 500; //TODO: change this. This will be the x coordinate of where we want the aruco marker to be at

        aruco::MarkerDetector detect;
        std::vector<aruco::Marker> Markers;
        cv::VideoCapture capture;
        cv::Mat image;

        std::vector<double> getWheelSpeeds(double angleOff, double baseSpeed);
        void updateAngle();
        void driveToMarker(int angleToMarker);
        bool trackMarker();

        int testMain();
        void mainLoop();

        //TODO: add GUI stuff
        //maybe add lidar stuff
};

#endif
