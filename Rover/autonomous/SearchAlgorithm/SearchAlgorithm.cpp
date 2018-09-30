#include <list>
#include <queue>
#include <set>
#include "SearchAlgorithm.h"

//Higher value means more avoidance from the algorithm
const double SearchAlgorithm::DISTWEIGHT = 1.0; //Weight given to the distance between two nodes when calculating cost
const double SearchAlgorithm::UPWEIGHT = 1.0; //Weight given to the difference in elevation when going up
const double SearchAlgorithm::DOWNWEIGHT = 1.0; //Weight given to the difference in elevation when going down
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
	//TODO: Find the x and y coordinates of the source and destination

	//Temporary variables, we need to figure out how we want to find their values
	int sourcex = source[0];
	int sourcey = source[1];
	int destx = dest[0];
	int desty = dest[1];

	//Determine the difference in latitude between the first two rows
	double latDiff = map[0][1].lat - map[0][0].lat; //Should be negative because 0 lat is equator

	//Find y coordinates
	sourcey = (source[0] - map[0][0].lat) / latDiff;
	desty = (dest[0] - map[0][0].lat) / latDiff;

	//Determine the difference in longitude between the first two columns
	double lngDiff = map[1][0].lng - map[0][0].lng; //should be negative becasuse longitude proceeds east to west in NA

	//Find x coordinates
	sourcex = (source[1] - map[0][0].lng) / lngDiff;
	destx = (dest[1] - map[0][0].lng) / lngDiff;

	//Create the source node and add it to the open list
	std::priority_queue<Node, std::vector<Node>, compareNodes> open; //Create open, closed, and register lists
	std::set<Node, compareNodes> closed;

	Node sourceNode(sourcex, sourcey, nullptr, 0.0, 0.0);
	Node destNode(destx, desty, nullptr, 0.0, 0.0); //CHANGE: in
	open.push(sourceNode);

	//Loop while there are still elements in the open list
	while (!open.empty()) {
		//Add the best element in the open list to the closed list
		Node current = open.top();
		open.pop();

		//if that element is the destination, we're done with the loop
		if (current == destNode) {
			destNode.parent = current.parent;
			break;
		}

		//if the current node is not in the closed list, add it, and add its neighbors to the open list
		if (closed.find(current) == closed.end()) {
			closed.insert(current);

			for (Node neighbor : getNeighbors(current, destx, desty)) {
				open.push(neighbor);
			}
		}
	}

	//create output list
	std::list<double*> out;
	Node interest = destNode;

	//ascend the parent tree, adding the corresponding GPS coordinates until we reach the source
	do {
		Cell cell = map[interest.x][interest.y];
		double pair[2] = { cell.lat, cell.lng };
		out.push_front(pair);
		interest = *interest.parent;
	} while (interest.parent != nullptr);

	//return output list
	return out;
}

std::list<SearchAlgorithm::Node> SearchAlgorithm::getNeighbors(Node& current, int destx, int desty) {
	std::list<Node> out;

	//for each neighbor, excluding the current node and any neighbors that go out of map bounds
	for (int x = current.x - 1; x < current.x + 1; x++) {
		for (int y = current.y - 1; y < current.y + 1; y++) {
			if (x < 0 || y < 0 || x > maxx || y > maxy || (x == current.x && y == current.y)) {
				continue;
			}

			//construct a new node for each neighbor and add it to the list
			double newG = getGCost(current, x, y);
			double newF = newG + getHeuristic(destx, desty, x, y);
			Node neighbor(x, y, &current, newG, newF);

			out.push_front(neighbor);
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
