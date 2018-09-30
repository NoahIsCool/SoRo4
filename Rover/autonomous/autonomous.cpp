#include "autonomous.h"

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
    //backs up a bit

    //turn like 15 degrees or so

    //drive forward a bit and we good(?)
}

void Autonomous::FindTennisBall()
{

}

double Autonomous::getAngleToTurn()
{

}

//Goes through all of the coordinates that we need to travel through
//Calls drive for the robot to smoothly reorient itself to from one node to the next
int Autonomous::MainLoop()
{
    std::vector<std::vector<double>> ListOfCheckpointsListOfCoords = generatePath();

    for(int i = 0; i < sizeof(ListOfCheckpointsListsOfCoords); i++) //loops through each of the checkpoints
    {
        for(int j = 0, j < sizeOf(ListOfCheckpointsListOfCoords[i]), j++) //loops through each of the coordinates to get to the next checkpoint
        {
            while(ListOfCheckpointsListOfCoords[i][j] != CurrentGPSHeading)
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
        //once arrives
        FindTennisBall();
    }
    cout << "We win!" << endl;
    return 0;
}


