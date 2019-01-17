#ifndef DRIVECONTROLLER_H
#define DRIVECONTROLLER_H

#include "drivecontroller_global.h"
#include <QObject>
#include <QWidget>

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

private slots:
    void on_resetButton_pressed();

private:
    Ui::DriveController *ui;
};

#endif // DRIVECONTROLLER_H
