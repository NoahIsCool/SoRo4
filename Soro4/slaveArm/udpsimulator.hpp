#ifndef UDPSIMULATOR_HPP
#define UDPSIMULATOR_HPP

#include <QObject>

/*
 * Dummy comms class for testing purposes.
 * Do not include into final build.
 */
class comms : public QObject {
    Q_OBJECT
public:
    comms(QObject* = nullptr);

public slots:
    void sendUDP(); // simulates udp input

private:
signals:
    void messageReady(QByteArray message); //emitted when a message is received


};

#endif // UDPSIMULATOR_HPP
