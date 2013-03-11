#include "mainwidget.h"

#include <QPushButton>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent)
{
    QPushButton * button = new QPushButton("Start Game", this);

    connect(button, SIGNAL(clicked()), this, SIGNAL(start()));
}
