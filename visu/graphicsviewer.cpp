#include "graphicsviewer.hpp"

GraphicsViewer::GraphicsViewer(QWidget *parent) :
    AbstractViewer(parent)
{
    _layout = new QHBoxLayout(this);
    setLayout(_layout);

    _scene = new QGraphicsScene(this);
    _view = new QGraphicsView(_scene, this);

    _layout->addWidget(_view);
}

void GraphicsViewer::onInit(int planetCount,
                            QVector<QVector<int> > distanceMatrix,
                            QVector<InitDisplayPlanet> planets,
                            QVector<QString> playerNicks,
                            int roundCount)
{
    _planets.resize(planetCount);
    _distance = distanceMatrix;


}

void GraphicsViewer::onTurn()
{
}
