#ifndef DEGREESMINUETSSEONDS_H
#define DEGREESMINUETSSEONDS_H

#include "QDialog"

class DegreesMinutesSeconds
{
private:
       double degrees;
       double minutes;
       double seconds;
       QString compassDirection;
public:
    DegreesMinutesSeconds(double deg, double min, double sec, QString compDir);
    ~DegreesMinutesSeconds();

    //getters
    double getDegrees();
    double getMinutes();
    double getSeconds();
    QString getCompassDirection();
};

#endif // DEGREESMINUETSSEONDS_H
