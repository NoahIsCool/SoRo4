#include "BallFinder.h"

BallFinder::BallFinder() {
  // this line usually takes a few seconds to execute
  //this->cam = new cv::VideoCapture(0); // open the default camera
  if (!this->cam.open(CAMERA)) {  // check if we succeeded
    // Todo deal with this error
    std::cerr << "FAILED TO OPEN CAMERA" << std::endl;
  }

  // for a workaround for an issue in opencv where it does not close the camera
  if(putenv("OPENCV_VIDEOIO_PRIORITY_MSMF=0"))
    std::cerr << "Warning: could not change environment variable. May not properly close webcam." << std::endl;

  // only takes control when found the ball
  this->hasControl = false;
  this->living = true;
  this->ball.distance = -1;
  this->ball.angle = -1;
  this->ball.h = -1;
  this->ball.w = -1;
  this->ball.x = -1;
  this->ball.y = -1;
  this->ball.z = -1;
  this->H_high = -1;
  this->S_high = -1;
  this->V_high = -1;
  this->H_low = -1;
  this->S_low = -1;
  this->V_low = -1;
  this->erosion = 0;
}

BallFinder::BallFinder(cv::VideoCapture * cam) {
  // only takes control when found the ball
  if(cam)
    this->cam = *cam;
  this->hasControl = false;
  this->living = true;
  this->ball.distance = -1;
  this->ball.angle = -1;
  this->ball.h = -1;
  this->ball.w = -1;
  this->ball.x = -1;
  this->ball.y = -1;
  this->ball.z = -1;
  this->H_high = -1;
  this->S_high = -1;
  this->V_high = -1;
  this->H_low = -1;
  this->S_low = -1;
  this->V_low = -1;
  this->erosion = 0;
}

// this is to be called by superior function to start the loop
void BallFinder::loop(BallFinder * inst) {
  char display[50];
  cv::Mat frame;
  cv::Mat original;
  if(DEBUG){
    cv::namedWindow("debug", cv::WINDOW_FREERATIO);
    cv::namedWindow("original", cv::WINDOW_FREERATIO);
    //cv::createTrackbar("H Low: ", "debug", &(inst->H_low), 255,)
  }

  do {
    // do the things
    inst->cam.read(frame);
    if(DEBUG) {
      original = frame;
      //cv::imshow("original", frame);
    }
    inst->convertImg(&frame);
    inst->findLargestContour(&frame);

    inst->hasControl = (inst->ball.distance == -1);
    if (DEBUG) {
      // draw shit
      cv::namedWindow("test", cv::WINDOW_FREERATIO);
      cv::createTrackbar("H Low: ",  "test", &(inst->H_low),   255);
      cv::createTrackbar("S Low: ",  "test", &(inst->V_low),   255);
      cv::createTrackbar("V Low: ",  "test", &(inst->S_low),   255);
      cv::createTrackbar("H High: ", "test", &(inst->H_high),  255);
      cv::createTrackbar("S High: ", "test", &(inst->S_high),  255);
      cv::createTrackbar("V High: ", "test", &(inst->V_high),  255);
      cv::createTrackbar("Erosion: ","test", &(inst->erosion), 255);

      sprintf(display, "Distance: %f, Angle: %f", inst->ball.distance, inst->ball.angle);
      cv::putText(original, display, cv::Point2f(0, 40), cv::FONT_HERSHEY_DUPLEX, 2 , cv::Scalar(0,255,255));
      cv::circle(original, cv::Point(inst->ball.x, inst->ball.y), inst->ball.w/2, cv::Scalar(0 , 255, 255));
      cv::imshow("original", original);
      cv::imshow("debug", frame);
      if(cv::waitKey(30) >= 0) {
        inst->living = false;
        break;
      }
    }
  } while (inst->living);
}/**/

void BallFinder::findLargestContour(cv::Mat *image) {
  std::vector<std::vector<cv::Point >> conts;
  std::vector<cv::Point> ret;
  cv::Rect boundingBox;
  double size = 0.0;
  double tmp;
  int largestIndex;
  cv::findContours(*image, conts, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
  if(conts.size() < 1){
    this->ball.distance = -1;
    return;
  }
  //size = cv::contourArea(conts[0]);

  for(auto& contour : conts){
    tmp = cv::contourArea(contour);
    if (tmp >= size){
      ret = contour;// could make faster by keeping track of only the index.(no copy constructor)
      size = tmp;
    }
  }/**/
  boundingBox = cv::boundingRect(ret);
  this->ball.w = boundingBox.width;
  this->ball.h = boundingBox.height;
  this->ball.x = boundingBox.x + boundingBox.width/2;
  this->ball.y = boundingBox.y + boundingBox.height/2;
  // TODO get the distance
  this->ball.distance = BALLRADIUS / tan(toRadians(getAngle(ball.w)));
  // find the signed pixels from the center then convert to an angle
  this->ball.angle = getAngle(H_RES - ball.x - (H_RES /2.0));

//    for(cv::Point point : v){
//      std::cout << point.x << ", " << point.y << " | ";
//    }
//    std::cout << std::endl;
}

/**
 * converts the input image to filter out only tennis ball colored objects. Set this.resolution to 0 ro
 * @param image
 */
void BallFinder::convertImg(cv::Mat *image) {
  cv::cvtColor(*image, *image, cv::COLOR_RGB2HSV);
  /*if(DEBUG){
    cv::namedWindow("debug", cv::WINDOW_FREERATIO);
    cv::imshow("debug", *image);
    cv::waitKey(0);
  }*/

  // ret should be binary( either 255, 255, 255 or 0, 0, 0,)
  cv::inRange(*image, cv::Scalar(H_low, S_low, V_low), cv::Scalar(H_high, S_high, V_high), *image);

  //errode and the dialate to remove noise and maintain erosion ( for math reasons)
  if(this->erosion > 0) {
    cv::Mat boxSize = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(erosion, erosion));
    cv::erode(*image, *image, boxSize);
    cv::dilate(*image, *image, boxSize);
  }
  //return;
}

std::ostream &operator<<(std::ostream &os, const BallFinder &finder) {
  os << "ball: " << finder.ball.x << ", " << finder.ball.y << ", "
     << finder.ball.distance << "; hasControl: " << finder.hasControl << std::endl;
  return os;
}

BallFinder::~BallFinder() {
  // technically done in VideoCapture destructor
  // if(cam)
  this->cam.release();
  if (DEBUG)
    cv::destroyAllWindows();
}


