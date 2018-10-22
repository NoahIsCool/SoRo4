// GPS Coordinates header
// GPSCoord. h

#ifndef GPS_COORD_H
#define GPS_COORD_H

#include <iostream>
#include <string>

class GPSCoord {

private:
	double latitude;										// latitude coordinate
	double longitude;										// longitude coordinate

public:
	GPSCoord();											// constructs GPS with default coordinates
	GPSCoord(double latitude, double longitude);		// constructs GPS with latitude and longitude coordinates
	virtual ~GPSCoord();								// frees array space as object is deleted

	double getLat();									// return latitude coordinate
	double getLong();									// return longitude coordinate

	void setLat(double latitudeCoord);					// set latitude coordinate 
	void setLong(double longitudeCoord);				// set longitude coordinate

	double convertGPS(std::string gpsInput);			// read GPS and convert to Decimal Degrees coordinate format
	double convertGPS(int deg, int min, double sec);	// read int GPS and convert to Decimal Degrees coordinate format

	double calcDistance(double lat1, double long1, double lat2, double long2);
	double calcHeading(double lat1, double long1, double lat2, double long2);

	double calculateDistance(GPSCoord destination);
	double calculateBearings(GPSCoord destination);
};
#endif
