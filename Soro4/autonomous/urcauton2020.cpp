#include "urcauton2020.h"

URCAuton2020::URCAuton2020()
{
    testGetWheelSpeedValues();
    //mainLoop();
}

URCAuton2020::getWheelSpeedValues(double amountOff, double baseSpeed)
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

URCAuton2020::testGetWheelSpeedValues()
{

}
