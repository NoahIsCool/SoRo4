#ifndef DRIVECONTROLLER_H
#define DRIVECONTROLLER_H

#include "drivecontroller_global.h"
#include <QObject>
#include <QWidget>
#include "gamepadmonitor.h"

namespace Ui {
class DriveController;
}

class DRIVECONTROLLERSHARED_EXPORT DriveController : public QWidget
{
    Q_OBJECT
public:
    explicit DriveController(QWidget *parent = nullptr);
    ~DriveController();
    void showWindow();

public slots:
    void addConsoleMessage(QString message);

private slots:
    void on_resetButton_pressed();

private:
    Ui::DriveController *ui;
    GamepadMonitor *myGamepadMonitor;
};

#endif // DRIVECONTROLLER_H
