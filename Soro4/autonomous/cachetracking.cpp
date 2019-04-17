#include "cachetracking.h"

using namespace cv;

CacheTracking::CacheTracking(std::string videoSource) : capture(videoSource)
{
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 1290);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
    if(!capture.isOpened()){
        //error in opening the video input
        std::cout << "Unable to open video file: " << videoSource << std::endl;
        exit(EXIT_FAILURE);
    }

    namedWindow("Frame");
    namedWindow("final");
    startTracking();
}
void CacheTracking::startTracking(){
    Mat frame;
    capture >> frame;
    Point center(capture.get(CV_CAP_PROP_FRAME_WIDTH)/2,capture.get(CV_CAP_PROP_FRAME_HEIGHT)/2);

    //calibrate
    double hLow = 0;
    double sLow = 0;
    double vLow = 0;
    double hHigh = 0;
    double sHigh = 0;
    double vHigh = 0;

    Mat hsv;
    Mat mask;
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    //range of green
                        //33, 80, 40      //102, 255, 255
    inRange(hsv, Scalar(79.333333333, 40, 40), Scalar(90, 255, 255), mask);
    std::vector<Rect> objects = findShapes(mask);
    //figure out object colors
    std::cout << objects.size() << std::endl;
    for(int i = 0; i < 50; i++){
        Mat RGB=hsv(Rect(objects[0].x + objects[0].width/2,objects[0].y + objects[0].height/2,1,1)); // use your x and y value
        //cvtColor(RGB, frame,CV_BGR2HSV);
        Vec3b hsvVector = RGB.at<Vec3b>(0,0);
        hLow += hsvVector.val[0]; //hue
        sLow += hsvVector.val[1]; //saturation
        vLow += hsvVector.val[2]; //value

        imshow("Frame", frame);
        imshow("final",mask);
        waitKey( 30 );
        capture >> frame;
        cvtColor(frame, hsv, COLOR_BGR2HSV);
    }
    hLow /= 50; //hue
    sLow /= 50; //saturation
    vLow /= 50; //value
    hHigh = hLow + 10;
    sHigh = sLow + 50;
    vHigh = vLow + 90;
    hLow -= 10; //hue
    sLow -= 50; //saturation
    vLow -= 90; //value
    std::cout << "got hsv values: " << hLow << " " << sLow << " " << vLow << std::endl;

    while(running){
        capture >> frame;

        cvtColor(frame, hsv, COLOR_BGR2HSV);
        //range of green
                            //33, 80, 40      //102, 255, 255
        //inRange(hsv, Scalar(79.333333333, 40, 40), Scalar(90, 255, 255), mask);
        inRange(hsv, Scalar(hLow, sLow, vLow), Scalar(hHigh, sHigh, vHigh), mask);

        /*erode(mask, mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate( mask, mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );*/

        //morphological closing (removes small holes from the foreground)

        //kernel
        /*Mat kernelOpen = Mat::ones(5,5, CV_32F);
        Mat kernelClose = Mat::ones(20,20,CV_32F);
        morphologyEx(mask,mask,cv::MORPH_OPEN,kernelOpen);
        morphologyEx(mask,mask,cv::MORPH_CLOSE,kernelClose);*/

        std::vector< Rect > objects = findShapes(mask);
        std::cout << "found: " << objects.size() << " objects" << std::endl;
        for(int i = 0; i < objects.size(); i++){
            //if(points.size() > 0 && area > 1000){
                //cv::Rect brect = cv::boundingRect(cv::Mat(points).reshape(2));
                cv::Rect brect = objects[i];
                cv::rectangle(frame, brect.tl(), brect.br(), cv::Scalar(100, 100, 200), 5, CV_AA);
                cv::rectangle(mask, brect.tl(), brect.br(), cv::Scalar(100, 100, 200), 5, CV_AA);
                Point objectCenter(brect.x+brect.width/2,brect.y+brect.height/2);
                cv::Rect centerRect = cv::boundingRect(cv::Mat(objectCenter).reshape(2));
                cv::rectangle(frame, centerRect.tl(), centerRect.br(), cv::Scalar(100, 100, 200), 5, CV_AA);
                cv::rectangle(mask, centerRect.tl(), centerRect.br(), cv::Scalar(100, 100, 200), 5, CV_AA);

                //distances are in pixels
                double x = abs(center.x - objectCenter.x);
                double y = abs(center.y - objectCenter.y);
                int up = (center.y - objectCenter.y) / y;
                int right = (center.x - objectCenter.x) / x;
                std::cout << "distance from object to center: " << x << " " << y << std::endl;
                int direction = NO_MOVE;
                if(abs(center.y - objectCenter.y) > centerThreshold){
                    if(up == 1){
                        direction = UP;
                    }else{
                        direction = DOWN;
                    }
                }
                if(abs(center.x - objectCenter.x) > centerThreshold){
                    if(right == 1){
                        if(direction == UP){
                            direction = UP_RIGHT;
                        }else if(direction == DOWN){
                            direction = DOWN_RIGHT;
                        }else{
                            direction = RIGHT;
                        }
                    }else{
                        if(direction == UP){
                            direction = UP_LEFT;
                        }else if(direction == DOWN){
                            direction = DOWN_LEFT;
                        }else{
                            direction = LEFT;
                        }
                    }
                }
                std::cout << "direction to move: ";
                switch(direction){
                case UP:
                    std::cout << "up";
                    break;
                case UP_RIGHT:
                    std::cout << "up right";
                    break;
                case RIGHT:
                    std::cout << "right";
                    break;
                case DOWN_RIGHT:
                    std::cout << "down right";
                    break;
                case DOWN:
                    std::cout << "down";
                    break;
                case DOWN_LEFT:
                    std::cout << "down left";
                    break;
                case LEFT:
                    std::cout << "left";
                    break;
                case UP_LEFT:
                    std::cout << "up left";
                    break;
                default:
                    std::cout << "perfectly balanced. Like all things should be";
                    break;
                };
                std::cout << std::endl;
            //}
        }

        cv::Rect centerDisplay = cv::boundingRect(cv::Mat(center).reshape(2));
        cv::rectangle(frame, centerDisplay.tl(), centerDisplay.br(), cv::Scalar(100, 100, 200), 10, CV_AA);
        cv::rectangle(mask, centerDisplay.tl(), centerDisplay.br(), cv::Scalar(100, 100, 200), 10, CV_AA);

        imshow("Frame", frame);
        imshow("final",mask);
        waitKey( 30 );
    }
    capture.release();
}

std::vector<Rect> CacheTracking::findShapes(Mat &frame, int minArea){
    std::vector< std::vector<Point> > contours;
    findContours(frame, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    std::vector<Rect> objects;

    for (int i = 0; i < contours.size(); i++) {
        std::vector<Point> points;
        for (int j = 0; j < contours[i].size(); j++) {
            Point p = contours[i][j];
            points.push_back(p);
        }
        double area = contourArea(contours[i]);

        if(area > minArea){
            objects.push_back(cv::boundingRect(cv::Mat(points).reshape(2)));
        }
    }

    return objects;
}
