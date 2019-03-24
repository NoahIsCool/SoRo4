#include "BallFinder.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <gtest/gtest.h>

int main(int argc, char **argv) {
  auto test = new BallFinder();
  test->living = false;

  test->H_low = 44;
  test->S_low = 60;
  test->V_low = 98;

  test->H_high = 86;
  test->S_high = 146;
  test->V_high = 255;

  test->erosion = 04;

  //auto slider = [](){};

  test->erosion = 0;
  test->living = true;
  std::thread loop(BallFinder::loop, test);
  /*while (true) {
    cv::namedWindow("test", cv::WINDOW_FREERATIO);
    cv::createTrackbar("H Low: ",  "test", &(test->H_low),   255);
    cv::createTrackbar("V Low: ",  "test", &(test->S_low),   255);
    cv::createTrackbar("S Low: ",  "test", &(test->V_low),   255);
    cv::createTrackbar("H High: ", "test", &(test->H_high),  255);
    cv::createTrackbar("S High: ", "test", &(test->S_high),  255);
    cv::createTrackbar("V High: ", "test", &(test->V_high),  255);
    cv::createTrackbar("Erosion: ","test", &(test->erosion), 255);

    if (cv::waitKey(30) >= 0)
      break;
  }*/
  //std::this_thread::sleep_for(std::chrono::seconds(300));
  //test->living = false;
  loop.join();
  
  return 0;
}