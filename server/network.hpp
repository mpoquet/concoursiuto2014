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
    struct Client
    {
        enum ClientType
        {
            UNKNOWN,
            PLAYER,
            DISPLAY
        };

        ClientType type;
        QByteArray buffer;

        Client() : type(UNKNOWN) {}
    };

    Q_OBJECT
public:
    explicit Network(quint16 port, int maxPlayerCount, int maxDisplayCount, QObject *parent = 0);

    void run();

    int playerCount() const;
    int displayCount() const;

signals:
    void loginPlayer(QTcpSocket * socket, QString nickname);
    void loginDisplay(QTcpSocket * socket);

    void movePlayer(QTcpSocket * socket,
                    QVector<int> planetsToScan,
                    QVector<BuildOrder> shipsToBuild,
                    QVector<ShipMove> shipsToMove);

    void clientDisconnected(QTcpSocket * socket);

public slots:
    void sendInitPlayer(QTcpSocket * socket,
                        int planetCount,
                        QVector<QVector<int> > distanceMatrix,
                        int roundCount,
                        int scanLimit);

    void sendFinished(QTcpSocket * socket, bool youWon);

private:
    void sendLoginPlayerACK(QTcpSocket * socket, char value);
    void sendLoginDisplayACK(QTcpSocket * socket, char value);

    void debugDisplayMove(QVector<int> planetsToScan,
                          QVector<BuildOrder> shipsToBuild,
                          QVector<ShipMove> shipsToMove);

private slots:
    void onNewConnection();
    void onMessageReceived();
    void onError(QAbstractSocket::SocketError error);
    void onDisconnected();

private:
    QTcpServer * _server;
    quint16 _port;
    QMap<QTcpSocket*, Client> _clients;

    QRegExp _regexMessage;
    QRegExp _regexLoginPlayer;
    QRegExp _regexMovePlayer;
    QRegExp _regexScans;
    QRegExp _regexBuilds;
    QRegExp _regexMoves;

    int _maxPlayerCount;
    int _maxDisplayCount;
};

#endif // NETWORK_HPP
