#include "sciencepackage.h"

SciencePackage::SciencePackage(QObject *parent) : QObject(parent), drillComm("science.conf"), monitor(nullptr, &drillComm)
{
    char date[18];
    time_t t = time(0);
    struct tm *tm;
    tm = localtime(&t);
    strftime(date, sizeof(date), "%Y-%m-%d_%H:%M", tm);
    csvFile.open(std::string("/opt/soonerRover/soro4/sciencePackage") + date + ".csv",std::ofstream::out);

    connect(&drillComm, SIGNAL(messageReady(QByteArray)), this, SLOT(handleMessage(QByteArray)));
    qDebug() << "all set up";
}

void SciencePackage::handleMessage(QByteArray message){
    //debug purposes
    /*qDebug() << "got message";
    for(quint8 next : message){
        csvFile << (uint)next << ",";
        qDebug() << next << ",";
    }*/
    //csvFile << std::endl;
    //std::cout << std::endl;
}

SciencePackage::~SciencePackage(){
    qDebug() << "closing science file";
    csvFile.flush();
    csvFile.close();
}
