#include "RoverMainWindow.h"
#include "ui_RoverMainWindow.h"

RoverMainWindow::RoverMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RoverMainWindow)
{
    ui->setupUi(this);
}

RoverMainWindow::~RoverMainWindow()
{
    delete ui;
}
