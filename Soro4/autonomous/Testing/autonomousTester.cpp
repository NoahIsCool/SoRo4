#include "autonomousTester.h"

autonomousTester::autonomousTester()
{
    cout << "Wheel speed test: " << testGetWheelSpeedValues() << endl;
    cout << "Input next cords test: " << testInputNextCords() << endl;

}

int autonomousTester::testGetWheelSpeedValues()
{
    //if the left wheel is turning the wrong way
    if(getWheelSpeedValues(10, baseSpeed)[0] < baseSpeed)
        return -1;

    //if the right wheel is turning the wrong way
    else if(getWheelSpeedValues(10, baseSpeed)[1] > baseSpeed)
        return -2;

    //if the robot does not attempt to correct faster the further off it is
    else if(getWheelSpeedValues(10, baseSpeed)[0] > getWheelSpeedValues(20, baseSpeed)[0])
        return -3;

    //the below tests are the same as the above ones except instead check if the angle is negative
    else if(getWheelSpeedValues(-10, baseSpeed)[1] < baseSpeed)
        return -4;

    else if(getWheelSpeedValues(-10, baseSpeed)[0] > baseSpeed)
        return -5;

    else if(getWheelSpeedValues(-10, baseSpeed)[1] > getWheelSpeedValues(-20, baseSpeed)[1])
        return -6;

    return 0
}

int autonomousTester::testInputNextCords()
{
    //checks to make sure that the cell is initialized properly without any errors
    Cell cell = inputNextCords();
    if(cell.gradient != -1)
        return -1;
    return 0;
}

int autonomousTester::testGetAngleToTurn()
{
    return 0; //not sure how to fake the GPS coordinates
}

int autonomousTester::testUpdateStatus()
{
    return 0; //again, not sure how to fake the GPS coordinates
}
