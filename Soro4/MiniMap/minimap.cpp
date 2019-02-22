#include "minimap.h"
#include "ui_minimap.h"

MiniMap::MiniMap(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiniMap)
{
    ui->setupUi(this);
}

MiniMap::~MiniMap()
{
    delete ui;
}
