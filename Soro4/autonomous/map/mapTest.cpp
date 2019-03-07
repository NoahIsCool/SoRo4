/*
Map processed in MATLAB beforehand so don't have to deal with now.
//#include <jpeglib.h>
//#include <jerror.h>
*/

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

struct Cell {
	float lat;
	float lng;
	unsigned char gradient;

	//double lat;
	//double lng;
	//double gradient;
};

int main() {
	cout << "line" << endl;
	/*//100 pixel avg 
	int colNum = 337;//width
	int rowNum = 172;//height
    */
	/*
	//for case of no pixel averaging
	int colNum = 3370;
	int rowNum = 1720;
	
	float lngW = -110.8655;
	float lngE = -110.72;
	float latN = 38.4356;
	float latS = 38.3765;
	*/
	//Norman
	int colNum = 522;
	int rowNum = 1035;

	float lngW = -97.450039;
	float lngE = -97.43323;
	float latN = 35.203796;
	float latS = 35.182157;
	 

	//ifstream data("mapAvgV1.csv");//100 pixel avg
	//ifstream data("mapNoAvg.csv");//no pixel averaging
	ifstream data("mapNorman.csv");
	string line;

	//static Cell map[1720][3370];//no pixel averaging
	//vector< vector<Cell> > map(1720, vector<Cell>(3370));
	vector< vector<Cell> > map(1035, vector<Cell>(522));
	//Cell map[172][337];//100 pixel avg

	//parse map value csv file
	vector<vector<string>> parsedCsv;
	while (getline(data, line))
	{
		stringstream lineStream(line);
		string cell;
		vector<string> parsedRow;
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
			map[row][col].gradient = stoi(parsedCsv[row][col]);
		}
	}

	//TODO: much more space efficient to not attach the gps coords to the average, and instead take gps 
		//coords, go to nearest pixel, and calculate average around that
		//possibly much less time efficient
		//gets around stack overflow problems



	//debug
	
	//for (int i = 0; i < rowNum; i++)
	for (int i = 0; i < 600; i++)
	{
		//cout << latN - (((latN - latS) / colNum)*(i + 0.5)) << "           ";
		cout << i << "          " << lngW - (((lngW - lngE) / rowNum)*(i + 0.5)) << "           ";
		cout << map[i][0].lat << ", " << map[i][0].lng << ", " << map[i][0].gradient << endl;
	}
	
}