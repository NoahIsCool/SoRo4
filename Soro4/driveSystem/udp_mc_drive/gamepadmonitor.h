#ifndef GAMEPADMONITOR_H
#define GAMEPADMONITOR_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include "comms.h"

QT_BEGIN_NAMESPACE
class QGamepad;
QT_END_NAMESPACE

class GamepadMonitor : public QObject
{
    Q_OBJECT
public:
    explicit GamepadMonitor(QObject *parent = nullptr);
    ~GamepadMonitor();

private:
    QGamepad *m_gamepad;
    comms *myComms;
    QTimer *udpTimer;
    bool cameraControl = false;
    // bytes that are actally sent
    qint8 modifiers = 0;
    qint8 leftSide = 0;
    qint8 rightSide = 0;
    qint8 gimbleUD = 0;
    qint8 gimbleRL = 0;

    void printVals();

public slots:
    void onLYAxis(double value);
    void onRXAxis(double value);
    void onRYAxis(double value);
    void onButtonRBumper(bool pressed);
    void onButtonLBumper(bool pressed);
    void onTriggerL(double value);
    void onTriggerR(double value);
    void onButtonUp(bool pressed);
    void onButtonDown(bool pressed);
    void onButtonLeft(bool pressed);
    void onButtonRight(bool pressed);
    void sendUDP();
};

#endif // GAMEPADMONITOR_H
