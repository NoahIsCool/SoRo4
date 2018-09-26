/*
*	Testing for the SearchAlgorithm class
*
*
*
*
*/

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

int main(int argc, char* argv[])
{
	std::string line;
	std::ifstream myfile("../../Rover/autonomous/SearchAlgorithm/Tests/PathfindingTests/test1.txt"); 
	
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

		int x = std::stoi(it[0]); //convert string to int
		int y = std::stoi(it[1]); 
		

		/*std::cout << "Map input:\n";
		while (getline(myfile, line))
		{		
			std::cout << line << '\n';

		}*/
		myfile.close();
	}

	return 0;
}