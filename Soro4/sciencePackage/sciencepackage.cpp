#include "sciencepackage.h"

SciencePackage::SciencePackage(QObject *parent) : QObject(parent), drillComm("science.conf"), monitor(nullptr, &drillComm)
{
    char date[14];
    time_t t = time(0);
    struct tm *tm;
    tm = localtime(&t);
    strftime(date, sizeof(date), "%Y-%m-%d_%H", tm);
    csvFile.open(std::string("sciencePackage") + date + ".csv");

    connect(&drillComm, SIGNAL(messageReady(QByteArray)), this, SLOT(handleMessage(QByteArray)));
}

void SciencePackage::handleMessage(QByteArray message){
    //debug purposes
    qDebug() << "got message";
    for(quint8 next : message){
        csvFile << next << ",";
        qDebug() << next << ",";
    }
    csvFile << std::endl;
    //std::cout << std::endl;
}

SciencePackage::~SciencePackage(){
    csvFile.flush();
    csvFile.close();
}
