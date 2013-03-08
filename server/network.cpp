#include "network.hpp"

#include <iostream>

using namespace std;

Network::Network(quint16 port, QObject *parent) :
    QObject(parent),
    _server(new QTcpServer(this)),
    _port(port)
{
    connect(_server, &QTcpServer::newConnection, this, &Network::onNewConnection);

    _regexMessage.setPattern("([ABC])(.*)");

    Q_ASSERT(_regexMessage.isValid());
    Q_ASSERT(_regexLoginPlayer.isValid());
    Q_ASSERT(_regexLoginDisplay.isValid());
    Q_ASSERT(_regexMovePlayer.isValid());
}

void Network::run()
{
    if (!_server->listen(QHostAddress::Any, _port))
        cerr << "Cannot listen port " << _port << endl;
    else
        cout << "Listening port " << _port << endl;
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

void Network::onNewConnection()
{
    QTcpSocket * socket = _server->nextPendingConnection();

    cout << QString("New connection from %1").arg(socket->peerAddress().toString()).toStdString() << endl;

    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(onMessageReceived()));

    _buffers[socket] = QByteArray();
}

void Network::onMessageReceived()
{
    QTcpSocket * socket = (QTcpSocket *) sender();

    _buffers[socket] += socket->readAll();

    int index;

    do
    {
        index = _buffers[socket].indexOf('\n');

        if (index != -1)
        {
            QByteArray message = _buffers[socket].left(index-1);
            _buffers[socket].remove(0, index+1);

            qDebug() << "Message received" << message;
        }

    } while (index != -1);
}

void Network::onError(QAbstractSocket::SocketError error)
{
    if (error != QAbstractSocket::RemoteHostClosedError)
        qDebug() << error;
}

void Network::onDisconnected()
{
    qDebug() << "Client disconnected";
}
