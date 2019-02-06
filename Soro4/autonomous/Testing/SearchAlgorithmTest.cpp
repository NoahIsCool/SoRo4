#include "../SearchAlgorithm.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

std::vector<std::string> tokenize(const std::string str, std::regex delim)
{
	std::sregex_token_iterator iter(str.begin(), str.end(), delim, -1);
	std::sregex_token_iterator end;

	std::vector<std::string> result(iter, end);

	return result;
}

int main()
{
    //Make a 3x3 map
	std::vector<std::vector<Cell>> map;

	for (int i = 0; i < 3; i++) {
		map.emplace(map.end());
		for (int j = 0; j < 3; j++) {
			map[i].emplace(map[i].end(), j, i, 0.0);
		}
	}

	SearchAlgorithm search(map, 3, 3, 1.0, 1000.0, 1000.0);
	Cell source(5, 5, 0.0);
	Cell dest(1, 1, 0.0);

	//confirm that exception handling is working properly
	try {
		search.findPath(source, dest);
	}
	catch (AStarException e) {
		std::cout << e.what();
	}

	//read in test map
	std::string line;
	std::ifstream myfile("test1.txt");

	if (!myfile.is_open())
	{
		std::cout << "Unable to open file";
	}

	std::regex reg("\\s*,\\s*"); //Regex matches any whitespace, 1 comma, any whitespace

								 //Get first line and set dimensions of map
	getline(myfile, line);
	std::vector<std::string> tokens = tokenize(line, reg);
	std::vector<std::string>::iterator it = tokens.begin();
	int sizeX = std::stoi(it[0]); //convert string to int
	int sizeY = std::stoi(it[1]);

	//Make new map. Not the most efficient way to do it, see
	//int *ary = new int[sizeX*sizeY];

	// ary[i][j] is then rewritten as
	//ary[i*sizeY + j]
	/*
	Cell **map2 = new Cell*[sizeY];
	for (int i = 0; i < sizeY; ++i) {
		map2[i] = new Cell[sizeX];
	}
	*/

	std::vector<std::vector<Cell>> map2;
	for (int i = 0; i < sizeY; i++) {
		map2.emplace(map2.end());
	}

	//Print and initialize map
	std::cout << "Map input:\n";
	for (int r = 0; r < sizeY; r++)
	{
		getline(myfile, line);
		tokens = tokenize(line, reg);
		it = tokens.begin();
		for (int c = 0; c < sizeX; c++)
		{
			Cell cell(r, c, stod(it[c]));
			//map2[c][r] = cell;  //CHNG r(lat), c(lng) to c(lng), r(lat)
			map2[c].emplace(map2[c].end(), r, c, stod(it[c]));
			std::cout << "(x:" << c << ", y:" << r << ")(lat:" << cell.lat << ", lng:" << cell.lng << "), " <<
				cell.gradient << " "; //CHNG clearly separate x,y and lat,lng. They should be opposite
		}
		std::cout << std::endl;
	}
	myfile.close(); //done with file

	//Get list for the path
	SearchAlgorithm search2(map2, sizeX, sizeY, 1.0, 1000.0, 1000.0);
	Cell start(4, 0, 0.0);
	Cell end(0, 4, 0.0);
	std::list<Cell> path = search2.findPath(start, end);
	for (Cell const& step : path) { //CHNG 'auto' to 'double*
		std::cout << "lat: " << step.lat << ", lng: " << step.lng << std::endl;
	}

	return 0;
}
