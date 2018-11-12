#include "slavearm.h"

SlaveArm::SlaveArm(QObject *parent) : QObject(parent) {

    // open serial connections
    QList<QSerialPortInfo> serialList = QSerialPortInfo::availablePorts();
    for (auto e : serialList)
    {
        QSerialPort* temp = new QSerialPort();
        temp->setPortName(e.portName());
        temp->setBaudRate(QSerialPort::Baud9600);

        if (temp->open(QIODevice::ReadWrite))
        {
            qDebug() << "Opened serial port" << e.portName();
            serialPorts.append(SerialPort{temp, 69});
            connect(temp, SIGNAL(readyRead()), this, SLOT(readSerial()));
            connect(temp, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError()));
        } else
        {
            qDebug() << "Error opening serial port:" << temp->errorString();
        }
    }

    // setup udp connection
    udpSocket = new comms("put_config_file_here", this);
    connect(udpSocket, SIGNAL(messageReady(QByteArray)), this, SLOT(readUDP(QByteArray)));

    // timer to handle signal loss
    signalTimer = new QTimer();
    signalTimer->start(2000);
    connect(signalTimer, SIGNAL(timeout()), this, SLOT(zeroDevices()));
}

void SlaveArm::readSerial() {
    // get incoming data
    QSerialPort* port = dynamic_cast<QSerialPort*>(sender());
    QByteArray message = port->readAll();

    //debug
    printf(message);

    //TODO: interpret message and do appropriate tasks.
}

void SlaveArm::readUDP(QByteArray message) {

    qDebug() << "Signal received: " << message << "\n";

    // mc computer is sending instructions
    if (message.at(0) == -127) {
        sendSerialMessage(message);
    }
    // mc computer is requesting data
    else if (message.at(0) == -126) {
        //TODO: send message to serial devices
    }
    // mc is requesting a list of the devices it recieves data from
    else if (message.at(0) == -125) {
        // TODO: send information and stuff
    }
}

// send a message to serial ports with ids specified in the message
void SlaveArm::sendSerialMessage(QByteArray message) {
    for (int i = 0; i < serialPorts.size(); ++i) {
        int id = serialPorts.at(i).id;
        if (id == message.at(1)) {
            serialPorts.at(i).port->write(message);
        } else if (id == 69) {
            //TODO: refresh serialIds
        }
    }
}

// probably not necessary
void SlaveArm::sendSerial(QSerialPort* port, QByteArray message) {
    port->write(message);
}

void SlaveArm::handleUDPSetupError(QString errorMessage) {
    qDebug() << errorMessage;
}

// Destructor. Not sure how memory management works with all these pointers.
SlaveArm::~SlaveArm() {
    udpSocket->~comms(); // is this necessary?
}

void SlaveArm::handleSerialError() {
    printf("serial error occured!");
}

/*
 * Sets the serial devices to a default state.
 * Note: This code is copied directly from last year's project. I don't know if the default values have changed since
 * then.
 */
void SlaveArm::zeroDevices() {
    for(int i = 0; i < serialPorts.size(); i++)
    {
        QByteArray stopBuffer;
        stopBuffer.append(char(-127));
        switch(serialPorts.at(i).id)
        {
        case 0:
            // drive
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            break;
        case 1:
            // arm
            stopBuffer.append(char(1));
            stopBuffer.append(char(90));
            stopBuffer.append(char(135));
            stopBuffer.append(char(135));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(175));
            stopBuffer.append(char(150));
            stopBuffer.append(char(97));
            break;
        case 2:
            // drill
            stopBuffer.append(char(2));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            break;
        default:
            // probably an uninitialized device id
            stopBuffer.append(serialPorts.at(i).id);
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            stopBuffer.append(char(0));
            break;
        }
        serialPorts.at(i).port->write(stopBuffer);
    }
}
