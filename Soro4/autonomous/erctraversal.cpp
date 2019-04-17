#include "erctraversal.h"

ERCTraversal::ERCTraversal()
{

}

//this returns wheel speeds to correct for the angle that the rover is off by
std::vector<double> Autonomous::getWheelSpeedValues(double amountOff, double baseSpeed)
{
    std::vector<double> PIDValues(2);
    amountOff /= .00015; //THIS IS A STUPID FIX FOR THE FIRST VERSION OF THE FORMULA. UNLESS IT WORKS AS IS, GET RID OF THIS LINE
    if(baseSpeed > 0)
    {
        //this formula works by taking the baseSpeed and increasing or decreasing it by a percent based off of amountOff
        //this formula is still almost certainly going to need to be adjusted
        PIDValues[0] = baseSpeed + baseSpeed * (1.045443e-16 + 0.00001087878*amountOff - 1.0889139999999999e-27*pow(amountOff, 2) + 7.591631000000001e-17*pow(amountOff, 3) - 7.105946999999999e-38*pow(amountOff, 4));
        PIDValues[1] = baseSpeed - baseSpeed * (1.045443e-16 + 0.00001087878*amountOff - 1.0889139999999999e-27*pow(amountOff, 2) + 7.591631000000001e-17*pow(amountOff, 3) - 7.105946999999999e-38*pow(amountOff, 4));
    }

    else
    {
        PIDValues[0] = baseSpeed - baseSpeed * (1.045443e-16 + 0.00001087878*amountOff - 1.0889139999999999e-27*pow(amountOff, 2) + 7.591631000000001e-17*pow(amountOff, 3) - 7.105946999999999e-38*pow(amountOff, 4));
        PIDValues[1] = baseSpeed + baseSpeed * (1.045443e-16 + 0.00001087878*amountOff - 1.0889139999999999e-27*pow(amountOff, 2) + 7.591631000000001e-17*pow(amountOff, 3) - 7.105946999999999e-38*pow(amountOff, 4));
    }

    return PIDValues;
}

void updateAngle()
{
    usleep(500000); //sleeps for half a second to make sure the rover is still
    biasZ = pos_llh.gyr_z; //It is really important that the rover is still here
    std::cout<< "Bias Z: " << biasZ << std::endl;

    while(true) //TODO: add killswitch or something for this
    {
        usleep(50000);
        currentAngle = currentAngle + (pos_llh.gyr_z - biasZ) * .1; //this is updated every 10th of a second. Assumes gyr_z is in correct unit already
        usleep(50000);
    }
}
