#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QByteArray>
#include <QRegExp>

#include "struct.hpp"

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(quint16 port, QObject *parent = 0);

    void run();
    
signals:
    void loginPlayer(QTcpSocket * socket, QString nickname);
    void loginDisplay(QTcpSocket * socket, QString nickname);

    void movePlayer(QTcpSocket * socket,
                    QVector<int> planetsToScan,
                    QVector<BuildOrder> shipsToBuild,
                    QVector<ShipMove> shipsToMove);

    void clientDisconnected(QTcpSocket * socket);
    
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

private slots:
    void onNewConnection();
    void onMessageReceived();
    void onError(QAbstractSocket::SocketError error);
    void onDisconnected();

private:
    QTcpServer * _server;
    quint16 _port;
    QMap<QTcpSocket*, QByteArray> _buffers;

    QRegExp _regexMessage;
    QRegExp _regexLoginPlayer;
    QRegExp _regexLoginDisplay;
    QRegExp _regexMovePlayer;
};

#endif // NETWORK_HPP
