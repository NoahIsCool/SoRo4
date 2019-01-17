#include "drivecontroller.h"
#include "ui_drivecontroller.h"

DriveController::DriveController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DriveController)
{
    ui->setupUi(this);
    ui->consoleList->addItem("Hello!");

    myGamepadMonitor = new GamepadMonitor();

    connect(myGamepadMonitor, SIGNAL(errorEnountered(QString)), ui->consoleList, SLOT(addItem(QString)));
}

DriveController::~DriveController()
{
    delete ui;
}

void DriveController::showWindow()
{
    this->show();
}

void DriveController::on_resetButton_pressed()
{
    ui->consoleList->clear();

    delete myGamepadMonitor;
    myGamepadMonitor = new GamepadMonitor();
}
