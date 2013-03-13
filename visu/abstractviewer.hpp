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

    virtual void onTurn() = 0; // to be defined
};

#endif // ABSTRACTVIEWER_HPP
