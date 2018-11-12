#include "drivewindow.h"
#include "ui_drivewindow.h"

driveWindow::driveWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::driveWindow)
{
    ui->setupUi(this);
}

driveWindow::~driveWindow()
{
    delete ui;
}
