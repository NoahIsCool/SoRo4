#include "drivecontroller.h"
#include "ui_drivecontroller.h"

DriveController::DriveController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DriveController)
{
    ui->setupUi(this);
    addConsoleMessage("Hello!");

    myGamepadMonitor = new GamepadMonitor(this);
    connect(myGamepadMonitor, SIGNAL(errorEncountered(QString)), this, SLOT(addConsoleMessage(QString)));
    myGamepadMonitor->init();
}

DriveController::~DriveController()
{
    delete ui;
}

void DriveController::showWindow()
{
    this->show();
}

void DriveController::addConsoleMessage(QString message)
{
    ui->consoleList->addItem(message);
}

void DriveController::on_resetButton_pressed()
{
    ui->consoleList->clear();

    disconnect(myGamepadMonitor, SIGNAL(errorEncountered(QString)), this, SLOT(addConsoleMessage(QString)));
    delete myGamepadMonitor;

    myGamepadMonitor = new GamepadMonitor(this);
    connect(myGamepadMonitor, SIGNAL(errorEncountered(QString)), this, SLOT(addConsoleMessage(QString)));
    myGamepadMonitor->init();
}
