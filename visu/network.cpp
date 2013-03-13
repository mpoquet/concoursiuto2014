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
            QString message = _buffer.left(index);
            _buffer.remove(0, index + 1);

            char type = message[0].toLatin1();

            switch(type)
            {
            case LOGIN_DISPLAY_ACK:
            {
                char response = message[1].toLatin1();

                if (response == OK)
                    emit logged();
                else if (response == NO_MORE_ROOM)
                    emit cannotLogIn();
                else
                    cerr << "Invalid LOGIN_DISPLAY_ACK received (" + message.toStdString() + ')' << endl;

            } break;
            case INIT_DISPLAY:
            {

            } break;
            case TURN_DISPLAY:
            {

            } break;
            default:
                cerr << "Unknown message received (" + message.toStdString() + ')' << endl;
            }

            if (message[0].toLatin1() == LOGIN_DISPLAY_ACK)
            {

            }
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
