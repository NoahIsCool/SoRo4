#include "drivewindow.h"
#include "ui_drivewindow.h"

driveWindow::driveWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::driveWindow)
{
    ui->setupUi(this);
    ui->consoleList->addItem("Hello!");
}

driveWindow::~driveWindow()
{
    delete ui;
}

void driveWindow::on_resetButton_pressed()
{
    ui->consoleList->clear();

}
