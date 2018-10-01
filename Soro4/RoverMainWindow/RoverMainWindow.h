#ifndef ROVERMAINWINDOW_H
#define ROVERMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class RoverMainWindow;
}

class RoverMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RoverMainWindow(QWidget *parent = nullptr);
    ~RoverMainWindow();

private:
    Ui::RoverMainWindow *ui;
};

#endif // ROVERMAINWINDOW_H
