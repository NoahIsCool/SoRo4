#include "erctraversal.h"

ERCTraversal::ERCTraversal()
{
    detect.setDictionary("erc.dict"); //this dictionary is in core
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 1290);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
    if(!capture.isOpened()){
        //error in opening the video input
        std::cout << "Unable to open video file: " << videoSource << std::endl;
        exit(EXIT_FAILURE);
    }

    namedWindow("Frame");
    namedWindow("final");
    testMain();
    //mainLoop();
}

int ERCTraversal::testMain()
{
    //std::thread statusThread(&ERCTraversal::updateAngle,this);
    while(true)
    {
        usleep(1000000);
        std::cout << gyro.gyr_z << std::endl;
    }
}

//this returns wheel speeds to correct for the angle that the rover is off by
std::vector<double> ERCTraversal::getWheelSpeeds(double angleOff, double baseSpeed)
{
    std::vector<double> PIDValues(2);
    angleOff /= .00015; //THIS IS A STUPID FIX FOR THE FIRST VERSION OF THE FORMULA. UNLESS IT WORKS AS IS, GET RID OF THIS LINE
    if(baseSpeed > 0)
    {
        //this formula works by taking the baseSpeed and increasing or decreasing it by a percent based off of amountOff
        //this formula is still almost certainly going to need to be adjusted
        PIDValues[0] = baseSpeed + baseSpeed * (1.045443e-16 + 0.00001087878*angleOff - 1.0889139999999999e-27*pow(angleOff, 2) + 7.591631000000001e-17*pow(angleOff, 3) - 7.105946999999999e-38*pow(angleOff, 4));
        PIDValues[1] = baseSpeed - baseSpeed * (1.045443e-16 + 0.00001087878*angleOff - 1.0889139999999999e-27*pow(angleOff, 2) + 7.591631000000001e-17*pow(angleOff, 3) - 7.105946999999999e-38*pow(angleOff, 4));
    }

    else
    {
        PIDValues[0] = baseSpeed - baseSpeed * (1.045443e-16 + 0.00001087878*angleOff - 1.0889139999999999e-27*pow(angleOff, 2) + 7.591631000000001e-17*pow(angleOff, 3) - 7.105946999999999e-38*pow(angleOff, 4));
        PIDValues[1] = baseSpeed + baseSpeed * (1.045443e-16 + 0.00001087878*angleOff - 1.0889139999999999e-27*pow(angleOff, 2) + 7.591631000000001e-17*pow(angleOff, 3) - 7.105946999999999e-38*pow(angleOff, 4));
    }

    std::cout << "left wheel speed: " << PIDValues[0] << "\nright wheel speed: " << PIDValues[1] << std::endl;
    return PIDValues;
}

void ERCTraversal::updateAngle()
{
    usleep(500000); //sleeps for half a second to make sure the rover is still
    biasZ = gyro.gyr_z; //It is really important that the rover is still here
    std::cout<< "Bias Z: " << biasZ << std::endl;

    while(true) //TODO: add killswitch or something for this
    {
        usleep(50000);
        std::cout << gyro.gyr_z << std::endl;
        currentAngle = currentAngle + (double)(gyro.gyr_z - biasZ) * .1; //this is updated every 10th of a second. Assumes gyr_z is in correct unit already
        usleep(50000);
    }
}

void ERCTraversal::mainLoop()
{
    driveToMarker(10);//NOTE: the final value for this won't be known untill september. For now we just need to test
    trackMarker();
    //TODO: add in a wait command here. We should probably create a universal command for that
    driveToMarker(20);//NOTE: the final value for this won't be known untill september. For now we just need to test
    trackMarker();
    //TODO: add in a wait command here. We should probably create a universal command for that
    driveToMarker(30);//NOTE: the final value for this won't be known untill september. For now we just need to test
    trackMarker();
    //TODO: add in a wait command here. We should probably create a universal command for that
    driveToMarker(40);//NOTE: the final value for this won't be known untill september. For now we just need to test
    trackMarker();
}

void ERCTraversal::driveToMarker(int angleToMarker)
{
    int timesDetected = 0; //should help filter out false readings
    while(timesDetected < 5)
    {
        capture >> image;
        Markers = detect.detect(image);
        if(Markers.size() == 0)
        {
            timesDetected++;
            break;
        }
        else
            timesDetected = 0;
        QByteArray array;
        array.append((char)-127);          //start message
        array.append((char)0);          //drive device ID is 0
        array.append((char)0);          //no modifiers
        array.append((char)getWheelSpeeds(angleToMarker-currentAngle, baseSpeed)[0]);     //left wheels
        array.append((char)getWheelSpeeds(angleToMarker-currentAngle, baseSpeed)[1]);     //right wheels
        array.append((char)0);          //gimble vertical
        array.append((char)0);          //gimble horizontal
        array.append((char)-2*speed/5); //hash - average of the previous 5 bytes
        mySocket.sendMessage(array);
        usleep(100000); //goes at this speed for .1 seconds. May need to go for longer
    }
}

void ERCTraversal::trackMarker()
{
    currentArea = (Markers[0][0].x - Markers[0][1].x) * (Markers[0][0].y - Markers[0][3].y);
    while(currentArea < stopArea)
    {
        //TODO: implement the tracking logic here
    }
}
