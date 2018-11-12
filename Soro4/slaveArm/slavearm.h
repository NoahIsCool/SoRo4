#ifndef SLAVEARM_H
#define SLAVEARM_H

#include "slavearm_global.h"
#include "../core/comms.h"
#include <QObject>
#include <QFile>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QList>

// contains a serial port and the arduino id associated with it.
struct SerialPort {
    QSerialPort* port;
    int8_t id;
};


/*
Handles communication routing between rover router and drive/arm/science arduinos.
Perhaps give this a more representative name (like CommunicationHub or something).
*/
class SLAVEARMSHARED_EXPORT SlaveArm : public QObject {
    Q_OBJECT
public:
    SlaveArm(QObject* = nullptr);
    ~SlaveArm();

private:
    QList<SerialPort> serialPorts;
    comms* udpSocket;
    QTimer* signalTimer;

    void sendSerial(QSerialPort*, QByteArray);
    void sendSerialMessage(QByteArray);

private slots:
    void readUDP(QByteArray); // handle the incomming udp data
    void readSerial(); // handle incoming serial data
    void handleUDPSetupError(QString);
    void handleSerialError();
    void zeroDevices();
};
#endif // SLAVEARM_H
