
#ifndef BALLFINDERMK1_BALLFINDER_H
#define BALLFINDERMK1_BALLFINDER_H
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <ostream>
#include <thread>
#include <math.h>

#define PI 3.1415926535897932384626433832795028841971693993751058209
// ( in meters )
#define BALLRADIUS .0635 / 2.0
#define H_RES 1920
#define DEBUG 1
#define CAMERA 0
#define MAX_SIZE 10
#define toRadians(theta) theta * PI / 180
#define toDegrees(theta) theta * 180 / PI
// 16/9 1080 => the fov is 1920 pixels wide and 70.42 degrees
#define getAngle(pixels) pixels * H_RES / 70.42

// distance set to -1 if no ball found
// if so then all others will be undefined
typedef struct {
  int x, y, z, w, h;
  // distance to the ball
  double distance;
  // the horizontal angle form the center of the camera
  double angle;
} Ball;

class BallFinder {
 public:
  Ball ball;
  bool hasControl;
  bool living;
  int H_low, S_low, V_low;
  int H_high, S_high, V_high;
  int erosion;

  BallFinder();
  BallFinder(cv::VideoCapture * cam);
  static void loop(BallFinder * inst);
  void convertImg(cv::Mat *image);
  friend std::ostream &operator<<(std::ostream &os, const BallFinder &finder);
  virtual ~BallFinder();

 private:
  cv::VideoCapture cam;
  void findLargestContour(cv::Mat *image);
};

#endif //BALLFINDERMK1_BALLFINDER_H
