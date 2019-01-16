#ifndef DRIVEWINDOW_H
#define DRIVEWINDOW_H

#include <QWidget>

namespace Ui {
class driveWindow;
}

class driveWindow : public QWidget
{
    Q_OBJECT

public:
    explicit driveWindow(QWidget *parent = nullptr);
    ~driveWindow();

private slots:
    void on_resetButton_pressed();

private:
    Ui::driveWindow *ui;
};

#endif // DRIVEWINDOW_H
