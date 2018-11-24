#include "RoverMainWindow.h"
#include "ui_RoverMainWindow.h"
#include "degreesminutesseconds.h"
#include <iostream>
#include <string>

RoverMainWindow::RoverMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RoverMainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->MainView);
    //ui->GPSWidget->layout()->setGeometry(QRect());

}

QString RoverMainWindow::converLatToDecimal(DegreesMinutesSeconds input)
{
    //DD = d + (min/60) + (sec/3600)
    double decimalDegrees = (input.getDegrees() + (input.getMinutes()/60) + (input.getSeconds()/3600));

    if(input.getCompassDirection() == "N" || input.getCompassDirection() == "n")
    {
        return QString::number(decimalDegrees);
    }
    else if (input.getCompassDirection() == "E" || input.getCompassDirection() == "e")
    {
        return QString::number(decimalDegrees);
    }
    else
    {
        std::cout << "This is the letter: ";
        std::cout << input.getCompassDirection().toStdString();
        decimalDegrees = decimalDegrees * -1;
        return QString::number(decimalDegrees);
    }

    //decimalFormat
}

void RoverMainWindow::submitClicked()
{
    double degrees;
    double minuets;
    double seconds;
    QString direction;


    degrees = getInput(ui->degreeInput1);
    minuets = getInput(ui->minInput1);
    seconds = getInput(ui->secondsInput1);
    direction = ui->dirInput1->text();
    DegreesMinutesSeconds northSouthDMS = DegreesMinutesSeconds(degrees,minuets,seconds,direction);

    degrees = getInput(ui->degInput2);
    minuets = getInput(ui->minInput2);
    seconds = getInput(ui->secInput2);
    direction = ui->dirInput2->text();
    DegreesMinutesSeconds eastWestDMS = DegreesMinutesSeconds(degrees,minuets,seconds,direction);

    QString output = converLatToDecimal(northSouthDMS) + " , " + converLatToDecimal(eastWestDMS);
    ui->decimalOutput_3->setText(output);
}

double RoverMainWindow::getInput(QLineEdit *textBox)
{
    if (textBox->text().isEmpty())
    {
        return 0.0;
    }
    else
    {
        return textBox->text().toDouble();
    }
}

RoverMainWindow::~RoverMainWindow()
{
    delete ui;
}
