/*
*	Testing for the SearchAlgorithm class
*
*
*
*
*/

#include "SearchAlgorithm.h"
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

int main(int argc, char* argv[])
{
	std::string line;
	std::ifstream myfile("test1.txt");

	if (!myfile.is_open())
	{
		std::cout << "Unable to open file";
	}
	else
	{
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
		Cell **map = new Cell*[sizeY];
		for (int i = 0; i < sizeY; ++i) {
			map[i] = new Cell[sizeX];
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
				Cell cell = {};
				cell.lng = c;
				cell.lat = r;
				cell.gradient = stod(it[c]);
				map[c][r] = cell;  //CHNG r(lat), c(lng) to c(lng), r(lat)
				std::cout << "(x:" << c << ", y:" << r << ")(lat:" << cell.lat << ", lng:" << cell.lng << "), " << 
					cell.gradient << " "; //CHNG clearly separate x,y and lat,lng. They should be opposite
			}
			std::cout << std::endl;
		}
		myfile.close(); //done with file

		//Get list for the path
		double start[] = { 4,0 };
		double end[] = { 0,4 };
		std::list<double*> path = SearchAlgorithm::findPath(start, end, map, sizeX, sizeY);
		for (double* const& step : path) { //CHNG 'auto' to 'double*
			std::cout << step[0] << ", " << step[1] << std::endl;
		}

	}

	return 0;
}