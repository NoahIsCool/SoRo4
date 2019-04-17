#ifndef CACHETRACKING_H
#define CACHETRACKING_H

#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

using namespace cv;

enum DIRECTION : int{
    NO_MOVE = 0,
    UP,
    UP_RIGHT,
    RIGHT,
    DOWN_RIGHT,
    DOWN,
    DOWN_LEFT,
    LEFT,
    UP_LEFT,
};

class CacheTracking
{
public:
    CacheTracking(std::string videoSource);

private:
    void startTracking();

    std::vector< Rect > findShapes(Mat &frame, int minArea = 1000);

    VideoCapture capture;
    Mat greenMask;
    bool running = true;
    double centerThreshold = 50;
};

#endif // CACHETRACKING_H
