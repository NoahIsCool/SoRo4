#include "balltracker.h"

BallTracker::BallTracker() : trackingThread(&BallTracker::ballTracking, this)
{
    //need to find all of the variables needed here
}

    //use opencv for all of this.
    //What we want but in python is here: https://github.com/NoahIsCool/SoRo3/blob/master/Autonomous/BallTracker.py
void BallTracker::ballTracking()
{
    //needs to connect to the camera here
    while(true)
    {
        //reads from the camera
        //converts the camera reading to HSV

        //filter the HSV values into binary: is there green pixels or not

        //filter out the noise of the HSV values

        //check to see if there is the tennis ball and continue if there isn't
        //if the area of the found ball is big enough then seize control from the main loop and track it
        //note: last year's tracking was done by seeing how far the ball was to the left or to the right of the robot and then tracking it

        //check if a kill button is pressed to stop it
    }
}

bool BallTracker::hasFound()
{
    return found;
}
