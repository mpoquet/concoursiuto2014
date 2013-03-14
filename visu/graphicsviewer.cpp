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

void GraphicsViewer::setPlayersColor()
{
    int h = 360 / _playerCount;
    int s = 0;
    int v = 0;

    for (int i = 0; i < _playerCount; ++i)
        _players[i+1].color = QColor::fromHsv(h * i, s, v);
}

void GraphicsViewer::onInit(int planetCount,
                            QVector<QVector<int> > distanceMatrix,
                            QVector<InitDisplayPlanet> planets,
                            QVector<QString> playerNicks,
                            int roundCount)
{
    _playerCount = playerNicks.size();
    _roundCount = roundCount;
    _currentRound = 0;

    _players.clear();
    _players[-1] = Player("Vide", Qt::white);
    _players[0] = Player("Autochtone", Qt::gray);

    for (int i = 0; i < playerNicks.size(); ++i)
        _players[i+1] = Player(playerNicks[i]);

    setPlayersColor();

    _planets.resize(planetCount);
    _distance = distanceMatrix;

    for (int i = 0; i < planetCount; ++i)
    {
        _planets[i].size = planets[i].planetSize;
        _planets[i].x = planets[i].posX;
        _planets[i].y = planets[i].posY;
        _planets[i].playerID = planets[i].playerID;
        _planets[i].shipCount = planets[i].shipCount;

        _planets[i].item = new QGraphicsEllipseItem(_planets[i].x - _planets[i].size / 2,
                                                    _planets[i].y - _planets[i].size / 2,
                                                    _planets[i].size / 2, _planets[i].size / 2);

    }
}

void GraphicsViewer::onTurn(QVector<int> scores,
                            QVector<TurnDisplayPlanet> planets,
                            QVector<ShipMovement> movements)
{
    for (int i = 0; i < scores.size(); ++i)
        _players[i+1].score = scores[i];

    for (int i = 0; i < _planets.size(); ++i)
    {
        _planets[i].playerID = planets[i].playerID;
        _planets[i].shipCount = planets[i].shipCount;
    }

    _movements = movements;
}
