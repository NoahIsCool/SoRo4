#include "minimap.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MiniMap w;
    w.show();

    return a.exec();
}
