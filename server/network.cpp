#include "network.hpp"

Network::Network(QObject *parent) :
    QObject(parent)
{
}


void Network::sendLoginPlayerACK(QTcpSocket *socket, bool ok)
{
}

void Network::sendLoginDisplayACK(QTcpSocket *socket, bool ok)
{
}

void Network::sendInitPlayer(QTcpSocket *socket,
                             int planetCount,
                             QVector<QVector<int> > distanceMatrix,
                             int roundCount,
                             int scanLimit)
{
}

void Network::sendFinished(QTcpSocket *socket, bool youWon)
{
}
