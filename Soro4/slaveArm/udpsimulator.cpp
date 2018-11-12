#include <iostream>
#include <string.h>
#include <vector>

#include "udpsimulator.hpp"

comms::comms(QObject *parent) : QObject(parent)
{
}

void comms::sendUDP() {
    /*int8_t header = -127;
    QByteArray message;
    std::string input("");
    while (input != "quit") {
        message.clear();
        std::cin >> input;
        std::cout << std::endl;
        message.append(header);
        message.append(input.data(), input.size());
        emit(messageReady(message));
    }*/

    // ignore this...
    int j = 0;
    int i = 0;
    for (i = 0; i < 100000000; ++i)
    {
        j += 1;
    }

    QByteArray message;
    message.append(-127);
    message.append('o');
    message.append('n');
    emit(messageReady(message));
}
