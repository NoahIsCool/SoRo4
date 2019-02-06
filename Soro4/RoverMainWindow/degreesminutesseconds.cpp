#include "degreesminutesseconds.h"

DegreesMinutesSeconds::DegreesMinutesSeconds(double deg, double min, double sec, QString compDir)
{
    degrees = deg;
    minutes = min;
    seconds = sec;
    compassDirection = compDir;
}

DegreesMinutesSeconds::~DegreesMinutesSeconds()
{

}

double DegreesMinutesSeconds::getDegrees()
{
    return degrees;
}

double DegreesMinutesSeconds::getMinutes()
{
    return minutes;
}

double DegreesMinutesSeconds::getSeconds()
{
    return seconds;
}

QString DegreesMinutesSeconds::getCompassDirection()
{
    return compassDirection;
}
