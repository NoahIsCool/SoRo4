#include "BallFinder.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <gtest/gtest.h>

auto test = new BallFinder();
void prep(){
  test->living = false;

  test->erosion = 00;
  test->V_high = 255;
  test->S_high = 255;
  test->H_high = 80;
  test->H_low = 50;
  test->S_low = 0;
  test->V_low = 0;
  auto slider = [](){};

}
 /*
TEST(constructor, existance) {

  ASSERT_TRUE(test);
  ASSERT_EQ(test->hasControl, false);
  ASSERT_EQ(test->ball.w, -1);
  ASSERT_EQ(test->ball.h, -1);
  ASSERT_EQ(test->ball.x, -1);
  ASSERT_EQ(test->ball.y, -1);
  ASSERT_EQ(test->ball.x, -1);
  ASSERT_EQ(test->ball.distance, -1);
  ASSERT_EQ(test->living, true);
  // TODO add any other variables here
  test->living = false;
  //TODO ASSERT_NO_THROW((*test)());
  ASSERT_NO_THROW(std::cout << (*test) << std::endl);
  // TODO add other public function;
  delete (test);
}*/

TEST(ConvertImage, adamjpg) {
  cv::Mat image = cv::imread("adam.jpg");
  cv::namedWindow( "Original", cv::WINDOW_FREERATIO );// Create a window for display.
  cv::namedWindow("Filtered", cv::WINDOW_FREERATIO);
  cv::imshow("Original", image);
  cv::VideoCapture * empty = nullptr;
  //int response = 0;

  test->convertImg(&image);
  cv::imshow("Filtered", image);
  cv::waitKey(0);
  // std::cout << "If the filter is correct enter a number > 1, else enter 0" << std::endl;
  // visual test
  ASSERT_NE(0, 1);
}

// apparently openCV only releases camera when the process exits

TEST(one, firstLoop){
  test->living = false;
  ASSERT_NO_THROW(BallFinder::loop(test));
}

/*
TEST(threading, tenSecondLoop){

  test->erosion = 0;
  test->living = true;
  std::thread loop(BallFinder::loop, test);
  std::this_thread::sleep_for(std::chrono::seconds(10));
  test->living = false;
  loop.join();


}*/

TEST(threading, contLoop){

  test->erosion = 0;
  test->living = true;
  std::thread loop(BallFinder::loop, test);
  while(true){
    cv::namedWindow("test", cv::WINDOW_FREERATIO);
    cv::createTrackbar("H Low: ", "test", &(test->H_low), 255);
    cv::createTrackbar("V Low: ", "test", &(test->S_low), 255);
    cv::createTrackbar("S Low: ", "test", &(test->V_low), 255);
    cv::createTrackbar("H High: ", "test", &(test->H_high), 255);
    cv::createTrackbar("S High: ", "test", &(test->S_high), 255);
    cv::createTrackbar("V High: ", "test", &(test->V_high), 255);
    cv::createTrackbar("Erosion: ", "test", &(test->erosion), 255);

    //cv::imshow("origonal", frame);
    if(cv::waitKey(30) >= 0)
      break;
  }
  test->living = false;
  loop.join();


}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  prep();
  // here you specify the test to be run. notice the trailing '*'
  // note that only one test that uses the camera can be run (opened in the default constructor)
  ::testing::GTEST_FLAG(filter) = "threading*";

  // start the test
  return RUN_ALL_TESTS();
}