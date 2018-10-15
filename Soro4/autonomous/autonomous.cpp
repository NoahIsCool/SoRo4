#include "autonomous.h"
#include <list>
#include <queue>
#include <set>
#include <unistd.h>

//this class assumes that the stuff to get the gpsHeading, the stuff to actually make the rover move, and everything needed for GeneratePath is available from another class.

Socket mySocket;
double speed = 60; //IDK what we want for speed right now or if we want to be updating it.

//Higher value means more avoidance from the algorithm
const double SearchAlgorithm::DISTWEIGHT = 1.0; //Weight given to the distance between two nodes when calculating cost
const double SearchAlgorithm::UPWEIGHT = 1000.0; //Weight given to the difference in elevation when going up
const double SearchAlgorithm::DOWNWEIGHT = 1000.0; //Weight given to the difference in elevation when going down
Cell** SearchAlgorithm::map; //Matrix of Cell objects
int SearchAlgorithm::maxx; //max x-value on the map
int SearchAlgorithm::maxy; //max y-value on the map

std::list<double*> SearchAlgorithm::findPath(double * source, double * dest, Cell ** map, int maxx, int maxy)
{
	//Change the static class members to their provided values
	SearchAlgorithm::map = map;
	SearchAlgorithm::maxx = maxx;
	SearchAlgorithm::maxy = maxy;

	return findPath(source, dest);
}

std::list<double*> SearchAlgorithm::findPath(double * source, double * dest)
{
	//Determine the x and y values of the source and destination from their latitude and longitude

	//Determine the difference in latitude between the first two rows
	double latDiff = map[0][1].lat - map[0][0].lat; //Should be negative because 0 lat is equator

	//Find y coordinates
	int sourcey = round((source[0] - map[0][0].lat) / latDiff);
	int desty = round((dest[0] - map[0][0].lat) / latDiff);

	//Determine the difference in longitude between the first two columns
	double lngDiff = map[1][0].lng - map[0][0].lng; //should be negative becasuse longitude proceeds east to west in NA

	//Find x coordinates
	int sourcex = round((source[1] - map[0][0].lng) / lngDiff);
	int destx = round((dest[1] - map[0][0].lng) / lngDiff);

	//Create the source node and add it to the open list
	std::priority_queue<Node, std::vector<Node>, compareNodes> open; //Create open, closed, and register lists
	std::set<Node, compareNodes2> closed;

	Node sourceNode(sourcex, sourcey, nullptr, 0.0, 0.0);
	Node destNode(destx, desty, nullptr, 0.0, 0.0); //CHNG 10/3: changed 'nullptr' to '&sourceNode'. This will change no-path output
	open.push(sourceNode);

	//Loop while there are still elements in the open list
	while (!open.empty()) {
		//Add the best element in the open list to the closed list
		Node * current = new Node(open.top()); //CHNG 10/3: to prevent a weird bug that made each node its own parent
		open.pop();

		//if that element is the destination, we're done with the loop
		if (*current == destNode) {
			destNode.parent = current->parent;
			break;
		}

		//if the current node is not in the closed list, add it, and add its neighbors to the open list
		std::pair<std::set<Node, compareNodes2>::iterator, bool> inserted = closed.insert(*current);
		if (inserted.second) {
			for (Node neighbor : getNeighbors(*current, destx, desty)) {
				open.push(neighbor);
			}
		}

		delete current;
	}

	//create output list
	std::list<double*> out;
	Node * interest = new Node(destNode);

	//ascend the parent tree, adding the corresponding GPS coordinates until we reach the source
	do {
		Cell cell = map[interest->x][interest->y];

		double * pair = new double[2]; //CHNG 10/5: dynamically allocated array to avoid the overwriting problem
		pair[0] = cell.lat;
		pair[1] = cell.lng;

		out.push_front(pair);

		interest = interest->parent;
	} while (interest != nullptr);

	//CHNG 10/3: interest interest might be assigned to a null pointer, so changed it to a pointer

	//return output list
	return out;
}

std::list<SearchAlgorithm::Node> SearchAlgorithm::getNeighbors(Node& current, int destx, int desty) {
	std::list<Node> out;

	//for each neighbor, excluding the current node and any neighbors that go out of map bounds
	for (int x = current.x - 1; x <= current.x + 1; x++) { //CHNG 'x < current.x + 1' to 'x <= current.x + 1'
		for (int y = current.y - 1; y <= current.y + 1; y++) { //CHNG 'y < current.y + 1' to 'x <= current.y + 1'
			if (x < 0 || y < 0 || x >= maxx || y >= maxy || (x == current.x && y == current.y)) {
				continue;
			}

			//construct a new node for each neighbor and add it to the list
			double newG = getGCost(current, x, y);
			double newF = newG + getHeuristic(destx, desty, x, y);

			Node * parent = new Node(current);
			Node * neighbor = new Node(x, y, parent, newG, newF);

			out.push_front(*neighbor);
		}
	}

	return out;
}

double SearchAlgorithm::getGCost(SearchAlgorithm::Node current, int x, int y) {
	//if the two nodes are adjacent, distance is 1, if they are diagonal, it is 1.4
	double distance = (abs((current.x + current.y) - (x + y)) == 1) ? 1.0 : 1.4;
	double gradientDiff = map[current.x][current.y].gradient - map[x][y].gradient;

	double gradientVal = 0.0;
	//Weight the gradient differently depending on if we are going up or down
	if (gradientDiff < 0) { //we're going up
		gradientVal = abs(gradientDiff) * UPWEIGHT;
	}
	else {
		gradientVal = gradientDiff * DOWNWEIGHT;
	}

	//return the cost so far plus the cost to move to the new node
	return current.g + (DISTWEIGHT * distance + gradientVal);
}

double SearchAlgorithm::getHeuristic(int destx, int desty, int x, int y) {
	//return the Manhattan distance between the point of interest and the destination point
	return (abs(destx - x) + abs(desty - y));
}

Autonomous::Autonomous()
{
    qInfo() << "library link test";

    MainLoop();
}

//return the speeds that the wheels need to move at to get to the next coordinate
std::Vector<double> Autonomous::getWheelSpeedsValues(double amountOff, double baseSpeed)
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
    usleep(5000);

    //turns for a few seconds to hopefully avoid the obsticle
    mySocket.sendUDP(0, 0, 0, -speed, speed, 0, 0, 0);
    usleep(5000);

    //drive forward a bit and continue(?)
    mySocket.sendUDP(0, 0, 0, speed, speed, 0, 0, speed);
    usleep(5000);
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
    //this can probably be done better by someone who is better at cpp than me
    //this is just so we can tell the robot to stop driving
    std::vector<double> killVector(2);
    killVector[0] = -1;
    killVector[1] = -1;

    std::vector<double> nextCords = inputNextCords(); //variable to hold the next coords that we need to travel to. Immediately calls the method to initialize them

    while(nextCords != killVector) //checks to make sure that we don't want to stop the loop
    {
        ListOfCoordsToNextCheckpoint = generatePath(path to nextCords); //TODO generates the path to the given set of coords
        for(int j = 0; j < sizeOf(ListOfCheckpointsListOfCoords); j++) //loops through each of the coordinates to get to the next checkpoint
        {
            while(ListOfCoordsToNextCheckpoint[i] != CurrentGPSHeading) //travels to the next set of coords. CurrentGPSHeading needs to be the range of coordinates that we want the roover to reach
            {
                if(IsThereObsticleOrStuck())
                {
                    avoidObsticle();
                }
                else //drives trying to get to the next checkpoint
                {
                    //find the angle that the robot needs to turn to to be heading in the right direction to hit the next coords
                    angleToTurn = getAngleToTurn();

                    std::vector<double> speeds = getWheelSpeedValues(angleToTurn, speed);
                    mySocket.sendUDP(0, 0, 0, speeds[0], speeds[1], 0, 0, (speeds[0] + speeds [1]) / 2);

                    usleep(500); //lets it drive for 500ms before continuing on
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


