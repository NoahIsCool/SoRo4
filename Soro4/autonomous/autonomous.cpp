#include "autonomous.h"

#define PI 3.14159265

//this class assumes that the stuff to get the gpsHeading, the stuff to actually make the rover move, and everything needed for GeneratePath is available from another class.
Autonomous::Autonomous() : mySocket("testConfig.conf")
{
	// this "should" make the comms object print out any errors it encounters to the terminal
    //FIXME: this is throwing some weird error
    //connect(mySocket, SIGNAL(messageReady(QByteArray)), this, SLOT(lidarValues(QByteArray)));
	qInfo() << "library link test";
	mainLoop();
}

//this function returns motor speeds so that while the rover is driving it is arcing to face the next set of coordinates (or just driving straight if its already facing the next set of coordinates)
//you put in how many degrees the rover needs to turn to face the next set of coords and the speed that you want the rover to drive at if it is directly facing the next set of coordinates
//For this to work best the base speed needs to be less than the maxium speed that the rover can drive at
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

//Returns a list of cells from the rover's current location to the specified destination
std::list<Cell> Autonomous::GeneratePath(Cell dest, SearchAlgorithm& alg)
{
	Cell source(pos_llh.lat, pos_llh.lon, 0.0);

	return alg.findPath(source, dest);
}

//updates all of the lidar readings and checks to see if an obstacle has been spotted
void Autonomous::lidarValues(QByteArray message)
{
    isThereObstacle =false;

    //updates the Lidar values and checks to see if there is an obstacle
    for(int i = 0; i < 5; i++)
    {
        obstacleDistances[i] = message[2*i] | message[2 * i - 1] << 8;
        if(obstacleDistances[i] < maxObstacleHeights[i])
            isThereObstacle = true;
    }

    //checks for holes. We may need to update this to check to make sure at least two Lidars see a hole before running avoidObstacle
    for(int i = 1; i < 4; i++)
    {
        if(obstacleDistances[i] > maxHoleDepths[i - 1])
            isThereObstacle = true;
    }
}

//Simply backs up, turns for a bit and then drives forward to before resuming normal operations if the robot is stuck or sees an obstacle
void Autonomous::avoidObstacle()
{
    //backs up for 5 seconds
    QByteArray array;
    array.append((char)-127);          //start message
    array.append((char)0);          //drive device ID is 0
    array.append((char)0);          //no modifiers
    array.append((char)-speed);     //left wheels
    array.append((char)-speed);     //right wheels
    array.append((char)0);          //gimble vertical
    array.append((char)0);          //gimble horizontal
    array.append((char)-2*speed/5); //hash - average of the previous 5 bytes
    mySocket.sendMessage(array);
    usleep(5000000);

    //turns for a few seconds to hopefully avoid the obsticle
    array.clear();
    array.append((char)-127);
    array.append((char)0);
    array.append((char)0);
    array.append((char)-speed);
    array.append((char)speed);
    array.append((char)0);
    array.append((char)0);
    array.append((char)0);
    mySocket.sendMessage(array);
    usleep(2000000);

    //drive forward a bit and continue(?)
    array.clear();
    array.append((char)-127); // start message
    array.append((char)0); // drive device ID is 0
    array.append((char)0); // no modifiers
    array.append((char)speed); // left wheels
    array.append((char)speed); // right wheels
    array.append((char)0); // gimble vertical
    array.append((char)0); // gimble horizontal
    array.append((char)(2*speed/5)); // hash - average of the previous 5 bytes
    mySocket.sendMessage(array);
    usleep(5000000);
}

//returns the difference between the current angle to the horizontal and the desired angle to reach the next cell
double Autonomous::getAngleToTurn(Cell next)
{
    double latitude = pos_llh.lat;
    double longitude = pos_llh.lon;
    double target = atan2(next.lat - latitude, next.lng - longitude) * 180 / PI; //atan2 takes in parameters (y,x)...
    double returnVal = angle - target;
    while(returnVal > 180)
        returnVal -= 360;
    while(returnVal < -180)
        returnVal += 360;
    return returnVal;
}

//This is meant to be run as a thread the whole time the autonomous program is running.
//This finds the angle that the rover is at and if the rover is stuck. This will be inaccurate if the rover does a pivot turn
//The way that it finds if the rover is stuck is if it is in the EXACT same position 6 times in a row (3 seconds) so this may need to be changed depending on the precision and noise of the GPS
//TODO add the accelerometer information here (probably(?))
//TODO need to change this to update every six times or something like that to filter out noise
void Autonomous::updateStatus()
{
    double longitude = pos_llh.lon;
    double latitude = pos_llh.lat;

    while(threadsRunning)
    {
        longitude = pos_llh.lon;
        latitude = pos_llh.lat;

        //if the robot hasn't moved
        if(lastLatitude == latitude && lastLongitude == longitude)
        {
            timesStuck++;
            if(timesStuck >= 6)
            {
                isStuck = true;
            }
        }

        else
        {
            isStuck = false;
            timesStuck = 0;
            angle = atan2(latitude - lastLatitude, longitude - lastLongitude) * 180 / PI;

            lastLatitude = latitude;
            lastLongitude = longitude;
            usleep(500000); //this sleep may need to be increased or decreased depending on how often we want the rover to update its angle
        }
    }
}

//TODO implement as a GUI function where we can input the next set of coordinates that the people tell us the tennis ball is
//right now just use cin to get the next coordinates
Cell Autonomous::inputNextCoords()
{
    Cell cell;
    std::cout << "enter next coordinate: lat long" << std::endl;
    std::cin >> cell.lat;
    std::cin >> cell.lng;
    cell.gradient = -1;
    return cell;
}

std::vector< std::vector<Cell> > Autonomous::parseMap(){
	int colNum = 522;
	int rowNum = 1035;

	float lngW = -97.450039;
	float lngE = -97.43323;
	float latN = 35.203796;
	float latS = 35.182157;
	 

	//ifstream data("mapAvgV1.csv");//100 pixel avg
	//ifstream data("mapNoAvg.csv");//no pixel averaging
    std::ifstream data("/opt/soonerRover/soro4/SoRo4/Soro4/autonomous/map/mapNorman.csv");
	std::string line;

	//static Cell map[1720][3370];//no pixel averaging
	//vector< vector<Cell> > map(1720, vector<Cell>(3370));
	std::vector< std::vector<Cell> > map(1035, std::vector<Cell>(522));
	//Cell map[172][337];//100 pixel avg

	//parse map value csv file
	std::vector<std::vector<std::string>> parsedCsv;
	while (getline(data, line))
	{
		std::stringstream lineStream(line);
		std::string cell;
		std::vector<std::string> parsedRow;
		while (getline(lineStream, cell, ','))
		{
			parsedRow.push_back(cell);
		}
		parsedCsv.push_back(parsedRow);
	}


	//attach gps coords to average blocks
	//coords should be the center of the block
	for (int row = 0; row < rowNum; row++)
	{
		for (int col = 0; col < colNum; col++)
		{
			//map[row][col].lat = latN - (((latN - latS) / colNum)*(col + 0.5));
			map[row][col].lat = latN - (((latN - latS) / colNum)*col);
			//map[row][col].lng = lngW - (((lngW - lngE) / rowNum)*(row + 0.5));
			map[row][col].lng = lngW - (((lngW - lngE) / rowNum)*	row);
			map[row][col].gradient = std::stoi(parsedCsv[row][col]);
		}
	}

	return map;
}

//Goes through all of the coordinates that we need to travel through
//Calls drive for the robot to smoothly reorient itself to from one node to the next
void Autonomous::mainLoop()
{
    map = parseMap();
    searcher = new SearchAlgorithm(map, map.size(), map[0].size(), 1.0, 1000.0, 1000.0);

    threadsRunning = true;
    std::thread statusThread(&Autonomous::updateStatus,this);

    //this is just so we can tell the robot to stop driving. We need to these into inputNextCoords to end the loop
    Cell killVector(-1, -1, 0.0);

    timesStuck = 0;
    isStuck = false;

    //used to calculate the angle to turn
    lastLatitude = pos_llh.lat;
    lastLongitude = pos_llh.lon;

    //BallTracker tennisTracker = new BallTracker(); //automatically starts a thread to track the tennisball
    Cell nextCords = inputNextCoords(); //variable to hold the next coords that we need to travel to. Immediately calls the method to initialize them
    std::cout << "next coords: " << nextCords.lat << " " << nextCords.lng << std::endl;

    while(nextCords != killVector) //checks to make sure that we don't want to stop the loop
    {
        std::list<Cell> path;
	try {
        path = GeneratePath(nextCords, *searcher);
	} catch (AStarException e) {
		nextCords = inputNextCoords(); //gets the next set of coords
		continue;
	}

        std::list<Cell>::iterator it = path.begin();

        //loops through each of the coordinates to get to the next checkpoint
        while(*it != nextCords) //travels to the next set of coords.
        {
            Cell currentGPS(pos_llh.lat, pos_llh.lon, 0.0);
            while(currentGPS != *it) //travels to the next set of coords. CurrentGPSHeading needs to be the range of coordinates that we want the rover to reach
            {
                if(isThereObstacle)
                {
                    avoidObstacle();
                }
                else
                {
                    //find the angle that the robot needs to turn to to be heading in the right direction to hit the next coords
                    double angleToTurn = getAngleToTurn(*it);

                    std::vector<double> speeds = getWheelSpeedValues(angleToTurn, speed);
                    std::vector<qint8> newSpeeds(speeds.size(),0);

                    //FIXME: change all speeds to ints not doubles. Dont need that accurate
                    //mySocket.sendUDP(0, 0, 0, speeds[0], speeds[1], 0, 0, (speeds[0] + speeds [1]) / 2);
                    QByteArray array;
                    array.append(-127); //start byte
                    array.append((char)0);
                    array.append((char)0);
                    array.append(newSpeeds[0]);
                    array.append(newSpeeds[1]);
                    array.append((char)0);
                    array.append((char)0);
                    array.append((char)(speeds[0] + speeds [1]) / 5);
                    mySocket.sendMessage(array);
                    usleep(500000); //lets it drive for 500ms before continuing on
                    currentGPS.lat = pos_llh.lat;
                    currentGPS.lng = pos_llh.lon;
                }
            }
			it++;
        }

        //wait for the tennisTracker to finish finding the tennis ball
        //OR if the tennis ball has not been found by the time that the rover got to the given checkpoints then we will pretend like we found the tennis ball and move on as if we had

        nextCords = inputNextCoords(); //gets the next set of coords
    }

    threadsRunning = false;
    statusThread.join();
    std::cout << "We win!" << std::endl;
}
