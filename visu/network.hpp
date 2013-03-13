#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <QObject>
#include <QTcpSocket>

#include "../server/struct.hpp"

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);
    
signals:
    void logged();
    void cannotLogIn();
    void disconnected();

    void initReceived(int planetCount,
                      QVector<QVector<int> > distanceMatrix,
                      QVector<InitDisplayPlanet> planets,
                      QVector<QString> playerNicks,
                      int roundCount);

private slots:
    void onError(QAbstractSocket::SocketError e);
    void onDisconnected();
    void onConnected();
    void onMessageReceived();

public slots:
    void connectToHost(const QString & address, quint16 port);
    void login();

private:
    QTcpSocket _socket;
    QByteArray _buffer;
};

#endif // NETWORK_HPP
