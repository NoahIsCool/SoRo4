#pragma once
#include <list>

/****
Placeholder struct, the interpretation team will provide final
*/
struct Cell {
	double lat;		//latitude of the cell
	double lng;		//longitude of the cell
	double gradient;	//average shade of the cell
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

		//Node constructor. Present simply for convinience, it just assigns the node variables
		Node(int x, int y, Node* parent, double g, double f) {
			(*this).x = x;
			(*this).y = y;
			(*this).parent = parent;
			(*this).g = g;
			(*this).f = f;
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
		bool operator() (Node n1, Node n2) {
			return n1.f > n2.f;
		}
	};

	//Higher value means more avoidance from the algorithm
	static const double DISTWEIGHT; //Weight given to the distance between two nodes when calculating cost
	static const double GRADWEIGHT; //Weight given to the difference in elevation between two nodes when calculating cost

	static Cell** map; //Matrix of Cell objects
	static int maxx; //max x-value on the map
	static int maxy; //max y-value on the map

	//Returns a list of Nodes adjacent or diagonal from the chosen node
	static std::list<Node> getNeighbors(Node& current, int destx, int desty);
	//Returns the cost to reach the node specified by x and y from the source node
	static double getGCost(Node current, int x, int y);
	//Returns the estimated cost to reach the destination node
	static double getHeuristic(int destx, int desty, int x, int y);

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
	static std::list<double*> findPath(double* source, double* dest, Cell** map, int maxx, int maxy);
	static std::list<double*> findPath(double* source, double* dest);
};