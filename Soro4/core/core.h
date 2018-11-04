#ifndef CORE_H
#define CORE_H

/*
Struct representing a cell of the topographic map used for autonomous pathfinding.
*/
struct Cell {
	double lat;		//latitude of the cell
	double lng;		//longitude of the cell
	double gradient;	//average shade of the cell

	bool operator==(Cell c) {
		return (lat == c.lat) && (lng == c.lng);
	}

    bool operator!=(Cell c){
        return (lat != c.lat) && (lng != c.lng);
    }
};

#endif // CORE_H
