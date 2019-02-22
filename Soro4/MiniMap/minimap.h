#ifndef MINIMAP_H
#define MINIMAP_H

#include <QWidget>

namespace Ui {
class MiniMap;
}

class MiniMap : public QWidget
{
    Q_OBJECT

public:
    explicit MiniMap(QWidget *parent = 0);
    ~MiniMap();

private:
    Ui::MiniMap *ui;
};

#endif // MINIMAP_H
