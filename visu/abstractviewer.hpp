#ifndef ABSTRACTVIEWER_HPP
#define ABSTRACTVIEWER_HPP

#include <QWidget>

#include "../server/struct.hpp"

class AbstractViewer : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractViewer(QWidget *parent = 0);
    virtual ~AbstractViewer();
signals:
    
public slots:
    virtual void onInit(int planetCount,
                        QVector<QVector<int> > distanceMatrix,
                        QVector<InitDisplayPlanet> planets,
                        QVector<QString> playerNicks,
                        int roundCount) = 0;

    virtual void onTurn(QVector<int> scores,
                        QVector<TurnDisplayPlanet> planets,
                        QVector<ShipMovement> movements) = 0;
};

#endif // ABSTRACTVIEWER_HPP
