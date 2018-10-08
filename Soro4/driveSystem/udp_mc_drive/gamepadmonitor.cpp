#include "gamepadmonitor.h"
#include <cmath>
#include <QtGamepad/QGamepad>
#include <QDebug>
#include <stdio.h>


GamepadMonitor::GamepadMonitor( QObject *parent)
    : QObject(parent)
    , m_gamepad(nullptr)
{
    // make sure gamepads are connected
    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    if (gamepads.isEmpty()) {
        qDebug() << "No gamepads detected!";
        return;
    }

    // attempt to set up communication ip's and stuff
    myComms = new comms("udp_mc_drive.conf");
    if(!myComms->isValid())
    {
        qDebug() << "Error initializing communicaions. Close and try again.";
        return;
    }

    // set up signals and slots to react to changes in xbox controller buttons/joysticks
    m_gamepad = new QGamepad(*gamepads.begin(), this);

    connect(m_gamepad, SIGNAL(axisLeftYChanged(double)), this, SLOT(onLYAxis(double)));
    connect(m_gamepad, SIGNAL(axisRightXChanged(double)), this, SLOT(onRXAxis(double)));
    connect(m_gamepad, SIGNAL(axisRightYChanged(double)), this, SLOT(onRYAxis(double)));

    connect(m_gamepad, SIGNAL(buttonR2Changed(double)), this, SLOT(onTriggerR(double)));
    connect(m_gamepad, SIGNAL(buttonL2Changed(double)), this, SLOT(onTriggerL(double)));

    connect(m_gamepad, SIGNAL(buttonR1Changed(bool)), this, SLOT(onButtonRBumper(bool)));
    connect(m_gamepad, SIGNAL(buttonL1Changed(bool)), this, SLOT(onButtonLBumper(bool)));

    connect(m_gamepad, SIGNAL(buttonUpChanged(bool)), this, SLOT(onButtonUp(bool)));
    connect(m_gamepad, SIGNAL(buttonDownChanged(bool)), this, SLOT(onButtonDown(bool)));
    connect(m_gamepad, SIGNAL(buttonLeftChanged(bool)), this, SLOT(onButtonLeft(bool)));
    connect(m_gamepad, SIGNAL(buttonRightChanged(bool)), this, SLOT(onButtonRight(bool)));


    // set up a timer to go off every 100 ms, and when that timer goes off send control data to rover
    udpTimer = new QTimer();
    connect(udpTimer, SIGNAL(timeout()), this, SLOT(sendUDP()));
    udpTimer->start(100);
}

void GamepadMonitor::onLYAxis(double value){
    if( !cameraControl )
    {
        // value (from controller) will be between -1 and 1
        // if your joystick characteristic equation is f(x) (has to have points (0,0) and (1,1) i.e. x^2),
        // then to include the deadzone you do: f( (x-deadzone) / (1-deadzone) )
        if (value > 0.1 ) {
            double x = (value-0.1)/0.9;
            leftSide = static_cast<qint8>(-round(x * x * 90.0));
        }
        else if (value < -0.1) {
            double x = (value+0.1)/0.9;
            leftSide = static_cast<qint8>(round(x * x * 90.0));
        }
        else {
            leftSide = 0;
        }
    }
}

void GamepadMonitor::onRYAxis(double value) {
    // if the left bumper is held down, the right stick will control the gimble
    if( cameraControl )
    {
        // value (from controller) will be between -1 and 1
        // if your joystick characteristic equation is f(x) (has to have points (0,0) and (1,1) i.e. x^2),
        // then to include the deadzone you do: f( (x-deadzone) / (1-deadzone) )
        if (value > 0.1 ) {
            double x = (value-0.1)/0.9;
            gimbleUD = static_cast<qint8>(-round(x * x * 5.0));
        }
        else if (value < -0.1) {
            double x = (value+0.1)/0.9;
            gimbleUD = static_cast<qint8>(round(x * x * 5.0));
        }
        else {
            gimbleUD = 0;
        }
    }
    else
    {
        // value (from controller) will be between -1 and 1
        // if your joystick characteristic equation is f(x) (has to have points (0,0) and (1,1) i.e. x^2),
        // then to include the deadzone you do: f( (x-deadzone) / (1-deadzone) )
        if (value > 0.1 ) {
            double x = (value-0.1)/0.9;
            rightSide = static_cast<qint8>(-round(x * x * 90.0));
        }
        else if (value < -0.1) {
            double x = (value+0.1)/0.9;
            rightSide = static_cast<qint8>(round(x * x * 90.0));
        }
        else {
            rightSide = 0;
        }
    }
}

void GamepadMonitor::onRXAxis(double value) {
    // if the left bumper is held down, the right stick will control the gimble
    if( cameraControl )
    {
        // value (from controller) will be between -1 and 1
        // if your joystick characteristic equation is f(x) (has to have points (0,0) and (1,1) i.e. x^2),
        // then to include the deadzone you do: f( (x-deadzone) / (1-deadzone) )
        if (value > 0.1 ) {
            double x = (value-0.1)/0.9;
            gimbleRL = static_cast<qint8>(-round(x * x * 5.0));
        }
        else if (value < -0.1) {
            double x = (value+0.1)/0.9;
            gimbleRL = static_cast<qint8>(round(x * x * 5.0));
        }
        else {
            gimbleRL = 0;
        }
    }
}

void GamepadMonitor::onButtonLBumper(bool pressed) {
    // if the left bumper is held down, the right stick will control the gimble
    if( pressed )
    {
        cameraControl = true;
        leftSide = 0;
        rightSide = 0;
    }
    else
    {
        cameraControl = false;
        gimbleRL = 0;
        gimbleUD = 0;
    }
}

void GamepadMonitor::onButtonRBumper(bool pressed) {
    // toggle break on button down
    if(pressed) {
         modifiers ^= (1 << 0);

         if( modifiers & (1 << 0) )
             qDebug() << "Break enabled";
         else
             qDebug() << "Break disabled";
     }
}

void GamepadMonitor::onTriggerL(double value) {
    // control rear modifier
    if(value > 0.5)
        modifiers |= (1 << 2);
    else
        modifiers &= ~(1 << 2);
}

void GamepadMonitor::onTriggerR(double value) {
    // control front modifier
    if(value > 0.5)
        modifiers |= (1 << 1);
    else
        modifiers &= ~(1 << 1);
}

void GamepadMonitor::onButtonUp(bool pressed) {
    // d pad does slow gimble control
    if(pressed){
        gimbleUD = 1;
    }else{
        gimbleUD = 0;
    }
}

void GamepadMonitor::onButtonDown(bool pressed) {
    // d pad does slow gimble control
    if(pressed){
        gimbleUD = -1;
    }else{
        gimbleUD = 0;
    }
}

void GamepadMonitor::onButtonLeft(bool pressed) {
    // d pad does slow gimble control
    if(pressed){
        gimbleRL = 1;
    }else{
        gimbleRL = 0;
    }
}

void GamepadMonitor::onButtonRight(bool pressed) {
    // d pad does slow gimble control
    if(pressed){
        gimbleRL = -1;
    }else{
        gimbleRL = 0;
    }
}

void GamepadMonitor::sendUDP() {
    // build the drive control message
    QByteArray out;
    out.append(char(-127));
    out.append(char(0));
    out.append(modifiers);
    out.append(leftSide);
    out.append(rightSide);
    out.append(gimbleUD);
    out.append(gimbleRL);
    qint8 hash = (modifiers + leftSide + rightSide + gimbleUD + gimbleRL)/5;
    out.append(hash);
    // send the drive control message
    myComms->sendMessage(out);
    printVals();
}

void GamepadMonitor::printVals() {
    // just for debugging
    int hash = (modifiers + leftSide + rightSide +gimbleUD + gimbleRL)/5;
    printf("%c, %d, %d, %d, %d, %d, %d, %d\n", char(-127), 0, modifiers, leftSide, rightSide, gimbleUD, gimbleRL, hash);
}

GamepadMonitor::~GamepadMonitor() {
    delete m_gamepad;
}

