#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <QObject>
#include <QTcpSocket>
#include <QVector>

#include "struct.hpp"

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);
    
signals:
    void loginPlayer(QTcpSocket * socket, QString nickname);
    void loginDisplay(QTcpSocket * socket, QString nickname);

    void movePlayer(QTcpSocket * socket,
                    QVector<int> planetsToScan,
                    QVector<BuildOrder> shipsToBuild,
                    QVector<ShipMove> shipsToMove);
    
public slots:
    void sendLoginPlayerACK(QTcpSocket * socket, bool ok);
    void sendLoginDisplayACK(QTcpSocket * socket, bool ok);

    void sendInitPlayer(QTcpSocket * socket,
                        int planetCount,
                        QVector<QVector<int> > distanceMatrix,
                        int roundCount,
                        int scanLimit);
    
    void sendFinished(QTcpSocket * socket,
                      bool youWon);
};

#endif // NETWORK_HPP
