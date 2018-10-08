#include Vector;
//A simple PID drive function for the robot to use to drive straight towards the node that it needs to go to
//returns the Current position of the robot at the end to see if its reached its target yet

Socket mySocket;
double leftWheelSpeed = 0;
double rightWheelSpeed = 0;

std::Vector<double> getPIDValues(double amountOff, double baseSpeed)
{
	std::Vector<double> PIDValues(2);
	
	if(baseSpeed > 0)
	{
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

GPSCoords Drive( GPS heading robot needs to go to, speed )
{
    //parse the GPS coordinates here to figure out how many degrees or radians we are off. We are probably going to have to fix the rover because of that...
    double angleToTurn = theParsedGPSCoordinates;
    std::Vector<double> speed = getPIDValues(angleToTurn, 60);
    
    mySocket.sendUDP(0, speed[0], speed[1], 0, 0);
    
    sleep(500);

    return currentGpsCoordinates;
}

//Goes through all of the coordinates that we need to travel through
//Calls drive for the robot to smoothly reorient itself to from one node to the next
int MainLoop()
{
    for(int i = 0; i < sizeof(GPSCoordsArray); i++)
    {
        while(Drive(GPSCoordsArray[i].heading()) != GPSCoordsArray[i].coordinates());
        //once arrives
    }
}

cout << "We are at the final node now" << endl;
