#include "sciencepackage.h"


SciencePackage::SciencePackage() : drillComm("science.conf"), monitor(nullptr, &drillComm)
{
    char date[14];
    time_t t = time(0);
    struct tm *tm;
    tm = localtime(&t);
    strftime(date, sizeof(date), "%Y-%m-%d_%H", tm);
    csvFile.open(std::string("sciencePackage") + date + ".csv");

    connect(&drillComm, SIGNAL(handleMessage(QByteArray)), this, SLOT(message(QByteArray)));
}


void SciencePackage::handleMessage(QByteArray message){
    //debug purposes
    for(quint8 next : message){
        csvFile << next << ",";
        std::cout << next << ",";
    }
    csvFile << std::endl;
    std::cout << std::endl;
}

SciencePackage::~SciencePackage(){
    csvFile.flush();
    csvFile.close();
}
