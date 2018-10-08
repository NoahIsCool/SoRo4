#include "autonomous.h"

//this class assumes that the stuff to get the gpsHeading, the stuff to actually make the rover move, and everything needed for GeneratePath is available from another class.

Socket mySocket;
double speed = 60; //IDK what we want for speed right now or if we want to be updating it.

Autonomous::Autonomous()
{
    qInfo() << "library link test";

    MainLoop();
}

//return the speeds that the wheels need to move at to get to the next coordinate
std::Vector<double> Autonomous::getPIDValues(double amountOff, double baseSpeed)
{
    std::Vector<double> PIDValues(2);

    if(baseSpeed > 0)
    {
        //this formula is still almost certainly going to need to be adjusted
        PIDValues[0] = speed + speed * (1.045443e-16 + 0.00001087878*amountOff - 1.0889139999999999e-27*pow(amountOff, 2) + 7.591631000000001e-17*pow(amountOff, 3) - 7.105946999999999e-38*pow(amountoff, 4);
        PIDValues[1] = speed - speed * (1.045443e-16 + 0.00001087878*amountOff - 1.0889139999999999e-27*pow(amountOff, 2) + 7.591631000000001e-17*pow(amountOff, 3) - 7.105946999999999e-38*pow(amountoff, 4);
    }

    else
    {
        PIDValues[0] = speed - speed * (1.045443e-16 + 0.00001087878*amountOff - 1.0889139999999999e-27*pow(amountOff, 2) + 7.591631000000001e-17*pow(amountOff, 3) - 7.105946999999999e-38*pow(amountoff, 4);
        PIDValues[1] = speed + speed * (1.045443e-16 + 0.00001087878*amountOff - 1.0889139999999999e-27*pow(amountOff, 2) + 7.591631000000001e-17*pow(amountOff, 3) - 7.105946999999999e-38*pow(amountoff, 4);
    }

    return PIDValues;
}

//not exactly sure what this will return
std::vector<std::vector<double>> Autonomous::GeneratePath()
{

}

//impliment much later
bool Autonomous::IsThereObsticleOrStuck()
{

}

void Autonomous::avoidObsticle()
{
    //backs up for 5 seconds
    mySocket.sendUDP(0, 0, 0, -speed, -speed, 0, 0, -speed);
    sleep(5000);

    //turns for a few seconds to hopefully avoid the obsticle
    mySocket.sendUDP(0, 0, 0, -speed, speed, 0, 0, 0);
    sleep(5000);

    //drive forward a bit and continue(?)
    mySocket.sendUDP(0, 0, 0, speed, speed, 0, 0, speed);
    sleep(5000);
}

void Autonomous::FindTennisBall()
{

}

double Autonomous::getAngleToTurn()
{

}

std::vector<double> Autonomous::inputNextCoords()
{

}

//Goes through all of the coordinates that we need to travel through
//Calls drive for the robot to smoothly reorient itself to from one node to the next
int Autonomous::MainLoop()
{
    std::vector<std::vector<double>> ListOfCoordsToNextCheckpoint = generatePath();

    //this can probably be done better by someone who is better at cpp than me
    //this is just so we can tell the robot to stop driving
    std::vector<double> killVector(2);
    killVector[0] = -1;
    killVector[1] = -1;

    std::vector<double> nextCords = inputNextCords(); //variable to hold the next coords that we need to travel to. Immediately calls the method to initialize them

    while(nextCords != killVector) //checks to make sure that we don't want to stop the loop
    {
        ListOfCoordsToNextCheckpoint = GeneratePath(path to nextCords); //generates the path to the given set of coords
        for(int j = 0; j < sizeOf(ListOfCheckpointsListOfCoords); j++) //loops through each of the coordinates to get to the next checkpoint
        {
            while(ListOfCoordsToNextCheckpoint[i][j] != CurrentGPSHeading) //travels to the next set of coords
            {
                if(IsThereObsticleOrStuck())
                {
                    avoidObsticle();
                }
                else //drives trying to get to the next checkpoint
                {
                    //find the angle that the robot needs to turn to to be heading in the right direction to hit the next coords
                    angleToTurn = getAngleToTurn();

                    std::vector<double> speeds = getPIDValues(angleToTurn, speed);
                    mySocket.sendUDP(0, 0, 0, speeds[0], speeds[1], 0, 0, (speeds[0] + speeds [1]) / 2);

                    sleep(500); //lets it drive for 500ms before continuing on
                }
            }
        }
        //once arrives to the checkpoint
        FindTennisBall();
        nextCords = inputNextCords(); //gets the next set of coords
    }
    cout << "We win!" << endl;
    return 0;
}


