#ifndef ERCTRAVERSAL_H
#define ERCTRAVERSAL_H


class ERCTraversal
{
    public:
        ERCTraversal();
    private:
        double currentAngle = 0;

        getWheelSpeeds(int angleOff);
        updateAngle();
        //add camera thread method
        //add lidar stuff
        //add GUI stuff
        //create a main loop that allows for us to very easily be able to change it to fit their map
};

#endif
