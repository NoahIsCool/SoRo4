#include "autonomousTester.h"

autonomousTester::autonomousTester()
{
    std::cout << "test wheel speed values result: " << testGetWheelSpeedValues() << std::endl;
    std::cout << "test input next cords result: " << testInputNextCords() << std::endl;
    std::cout << "test get angle to turn result: " << testGetAngleToTurn() << std::endl;
    std::cout << "test update status result : " << testUpdateStatus() << std::endl;

}

int autonomousTester::testGetWheelSpeedValues()
{
    //if the left wheel is turning the wrong way
    if(getWheelSpeedValues(10, speed)[0] < speed)
        return -1;

    //if the right wheel is turning the wrong way
    else if(getWheelSpeedValues(10, speed)[1] > speed)
        return -2;

    //if the robot does not attempt to correct faster the further off it is
    else if(getWheelSpeedValues(10, speed)[0] > getWheelSpeedValues(20, speed)[0])
        return -3;

    //the below tests are the same as the above ones except instead check if the angle is negative
    else if(getWheelSpeedValues(-10, speed)[1] < speed)
        return -4;

    else if(getWheelSpeedValues(-10, speed)[0] > speed)
        return -5;

    else if(getWheelSpeedValues(-10, speed)[1] > getWheelSpeedValues(-20, speed)[1])
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
