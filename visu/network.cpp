#include "network.hpp"

#include <iostream>

#include "../server/protocole.h"

using namespace std;

Network::Network(QObject *parent) :
    QObject(parent)
{
    connect(&_socket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(&_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(&_socket, SIGNAL(readyRead()), this, SLOT(onMessageReceived()));
    connect(&_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
}

void Network::onError(QAbstractSocket::SocketError e)
{
    if (e != QAbstractSocket::RemoteHostClosedError)
        qDebug() << e;
}

void Network::onDisconnected()
{
    cout << "Disconnected" << endl;
}

void Network::onConnected()
{
    cout << "Connected" << endl;
}

void Network::onMessageReceived()
{
    _buffer += _socket.readAll();

    int index = 0;

    do
    {
        index = _buffer.indexOf('\n');

        if (index != -1)
        {

        }
    } while (index != -1);
}

void Network::connectToHost(const QString &address, quint16 port)
{
    _socket.connectToHost(address, port);
}

void Network::login()
{
    if (_socket.state() != QAbstractSocket::ConnectedState)
    {
        cerr << "Bad Network::login call : socket is not connected" << endl;
        return;
    }

    QByteArray qba;

    qba += LOGIN_DISPLAY;
    qba += '\n';
}
