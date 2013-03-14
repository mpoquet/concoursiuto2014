#ifndef GRAPHICSVIEWER_HPP
#define GRAPHICSVIEWER_HPP

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QGraphicsItem>
#include <QHash>

#include "abstractviewer.hpp"

class GraphicsViewer : public AbstractViewer
{
    Q_OBJECT

    struct Player
    {
        QString nick;
        QColor color;
        int score;

        Player(const QString & n = "???", const QColor & c = Qt::black) :
            nick(n), color(c) {}
    };

    struct Planet
    {
        QGraphicsEllipseItem * item;
        int playerID;
        int size;
        int x;
        int y;
        int shipCount;
    };

public:
    explicit GraphicsViewer(QWidget *parent = 0);
    
private:
    void setPlayersColor();

signals:
    
public slots:
    void onInit(int planetCount,
                QVector<QVector<int> > distanceMatrix,
                QVector<InitDisplayPlanet> planets,
                QVector<QString> playerNicks,
                int roundCount);

    void onTurn(QVector<int> scores,
                QVector<TurnDisplayPlanet> planets,
                QVector<ShipMovement> movements);

private slots:
    void redraw();

private:
    QHBoxLayout * _layout;

    QGraphicsScene * _scene;
    QGraphicsView * _view;

    QVector<Planet> _planets;
    QVector<QGraphicsEllipseItem *> _movements;
    QVector<QVector<int> > _distance;
    int _roundCount;
    int _currentRound;
    int _playerCount;

    QHash<int, Player> _players; // playerID -> Player
};

#endif // GRAPHICSVIEWER_HPP
