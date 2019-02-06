#pragma once

#include "core/core.h"
#include <list>
#include <stdexcept>
#include <string>
#include <queue>
#include <set>
#include <vector>

class AStarException : public std::runtime_error {
public:
	AStarException(const std::string& message) : runtime_error(message) {};
};

class SearchAlgorithm {
private:

	/****
	Class representing a cell on a topographic map. Used for a* pathfinding
	*/
	struct Node {
		//Coordinates of the node in the grid
		int x;
		int y;

		//Parent node, used for backtracing after a* has found a path
		Node* parent;

		//The cost to move from the source to this node
		double g;

		//The score used to select the next node from the open list
		double f;

		Node() {
			x = 0;
			y = 0;
			parent = nullptr;
			g = 0.0;
			f = 0.0;
		}

		//Node constructor. Present simply for convinience, it just assigns the node variables
		Node(int x, int y, Node* parent, double g, double f) {
			(*this).x = x;
			(*this).y = y;
			(*this).parent = parent;
			(*this).g = g;
			(*this).f = f;
		}

		//Copy constructor utilizing overloaded = operator
		Node(const Node &copy) { //CHNG 10/3: added
			(*this) = copy;
		}

		//Cleans up the whole list
		~Node() {
			delete parent;
		}

		//Overloaded = operator for Node object. Copies the entire list instead of just the value of the pointer.
		void operator= (const Node& copy) {
			x = copy.x;
			y = copy.y;

			if (copy.parent != nullptr) {
				parent = new Node(*copy.parent);
			}
			else {
				parent = nullptr;
			}

			g = copy.g;
			f = copy.f;
		}

		//Overloaded == operator for node. Compares nodes based on coordinate
		bool operator== (Node n) {
			return ((*this).x == n.x) && ((*this).y == n.y);
		}
	};

	/****
	Comparator class for a min priority queue of Nodes. Compares nodes based on their f value.
	*/
	struct compareNodes {
		bool operator() (const Node& n1, const Node& n2) const {
			return n1.f > n2.f;
		}
	};

	/*****
	Comparator class for set of Nodes. Compares nodes based on their x and y values.
	*/
	struct compareNodes2 {
		bool operator() (const Node& n1, const Node& n2) const {
			if (n1.x == n2.x) {
				return n1.y < n2.y;
			}
			else {
				return n1.x < n2.x;
			}
		}
	};

	//Higher value means more avoidance from the algorithm
	double DISTWEIGHT; //Weight given to the distance between two nodes when calculating cost
	double UPWEIGHT; //Weight given to the difference in elevation when going up
	double DOWNWEIGHT; //Weight given to the difference in elevation when going down

	std::vector<std::vector<Cell>> map; //Matrix of Cell objects
	int maxx; //max x-value on the map
	int maxy; //max y-value on the map

	//Returns a list of Nodes adjacent or diagonal from the chosen node
	std::list<Node> getNeighbors(Node& current, int destx, int desty);
	//Returns the cost to reach the node specified by x and y from the source node
	double getGCost(Node current, int x, int y);
	//Returns the estimated cost to reach the destination node
	double getHeuristic(int destx, int desty, int x, int y);

public:

	/****
	Performs a* pathfinding over the topographic map and returns the best path. The first method must be run first to
	double* source - latitude and longitude of the source in decimal degrees, expected in the format: latutude, longitude
	double* dest - latitude and longitude of the destination in decimal degrees, expected in the format: latitude, longitude
	Cell** map - a matrix of cell objects representing the topographic map
	int maxx - the maximum x value of the matrix
	int maxy - the maximum y value of the matrix
	return - a list of GPS coordinate pairs 50 meters apart forming a path from the source to the destination
	*/
	SearchAlgorithm(std::vector<std::vector<Cell>> map, int maxx, int maxy, double distWeight, double upWeight, double downWeight);
	std::list<Cell> findPath(Cell source, Cell dest);
};
