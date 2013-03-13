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
    };

public:
    explicit GraphicsViewer(QWidget *parent = 0);
    
signals:
    
public slots:
    void onInit(int planetCount,
                QVector<QVector<int> > distanceMatrix,
                QVector<InitDisplayPlanet> planets,
                QVector<QString> playerNicks,
                int roundCount);

    void onTurn();
private:
    QHBoxLayout * _layout;

    QGraphicsScene * _scene;
    QGraphicsView * _view;

    QVector<QGraphicsItem*> _planets;
    QVector<QVector<int> > _distance;
    int _roundCount;
    int _currentRound;

    QHash<int, Player> _players; // playerID -> Player
};

#endif // GRAPHICSVIEWER_HPP
