#include "sciencepackage.h"


SciencePackage::SciencePackage() : drillComm("science.conf"), monitor(nullptr, &drillComm)
{
    connect(&drillComm, SIGNAL(handleMessage(QByteArray)), this, SLOT(message(QByteArray)));
}


void handleMessage(QByteArray message){
    for(quint8 next : message){
        std::cout << next << ",";
    }
    std::cout << std::endl;
}
