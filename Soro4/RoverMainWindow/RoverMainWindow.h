#ifndef ROVERMAINWINDOW_H
#define ROVERMAINWINDOW_H

#include <QMainWindow>
#include <degreesminutesseconds.h>
#include <QLineEdit>

namespace Ui {
class RoverMainWindow;
}

class RoverMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RoverMainWindow(QWidget *parent = nullptr);
    QString converLatToDecimal(DegreesMinutesSeconds input);
    double getInput(QLineEdit *textBox);
    ~RoverMainWindow();

private slots:
    void submitClicked();

private:
    Ui::RoverMainWindow *ui;
};

#endif // ROVERMAINWINDOW_H
