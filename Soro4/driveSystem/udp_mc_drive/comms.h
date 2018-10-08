#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QUdpSocket>
#include <QFile>

class comms : public QObject
{
    Q_OBJECT
public:
    explicit comms(const char* config_file, QObject *parent = nullptr);
    void sendMessage(QByteArray message);
    bool isValid() { return valid; } // validity detemined in constructor, is immutable

signals:
        void messageReady(QByteArray message); // emitted whenever a message is recieved

private:
    QString send_ip;
    quint16 send_port;
    QString recieve_ip;
    quint16 recieve_port;
    QUdpSocket* udpSocket;
    bool readConfig(const char* filename); // returns true on success, false on failure
    bool valid;

private slots:
    void readMessage();
};

#endif // SOCKET_H
