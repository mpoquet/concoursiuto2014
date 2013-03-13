#include "network.hpp"

#include <iostream>

#include <QStringList>

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
    {
        qDebug() << e;
        emit error();
    }
}

void Network::onDisconnected()
{
    cout << "Disconnected" << endl;

    emit disconnected();
}

void Network::onConnected()
{
    cout << "Connected" << endl;

    emit connected();
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
                else if (response == ALREADY_LOGGED)
                    qDebug() << "I can't log twice...";
                else
                    cerr << "Invalid LOGIN_DISPLAY_ACK received (" + message.toStdString() + ')' << endl;

            } break;
            case INIT_DISPLAY:
            {
                QStringList qsl = message.split(SEP);
                QStringList qsl2;

                int planetCount;
                QVector<QVector<int> > distanceMatrix;
                QVector<InitDisplayPlanet> planets;
                QVector<QString> playerNicks;
                int roundCount;

                planetCount = qsl.at(0).right(qsl.at(0).size() - 1).toInt();
                qsl2 = qsl.at(1).split(SSEP);

                if (qsl2.size() != planetCount * planetCount)
                {
                    cerr << "Invalid distance matrix size" << endl;
                    continue;
                }

                distanceMatrix.resize(planetCount);
                for (int y = 0, i = 0; y < planetCount; ++y)
                {
                    distanceMatrix[y].resize(planetCount);
                    for (int x = 0; x < planetCount; ++x)
                        distanceMatrix[y][x] = qsl2.at(i++).toInt();
                }

                qsl2 = qsl.at(2).split(SSEP);

                if (qsl2.size() != planetCount * 5)
                {
                    cerr << "Invalid planets size" << endl;
                    continue;
                }

                planets.resize(planetCount);
                for (int i = 0; i < planetCount; ++i)
                {
                    planets[i].posX = qsl2[5*i].toInt();
                    planets[i].posY = qsl2[5*i+1].toInt();
                    planets[i].playerID = qsl2[5*i+2].toInt();
                    planets[i].shipCount = qsl2[5*i+3].toInt();
                    planets[i].planetSize = qsl2[5*i+4].toInt();
                }

                qsl2 = qsl.at(3).split(SSEP);

                if (qsl2.at(0).toInt() != qsl2.size() - 1)
                {
                    cerr << "Invalid player nicks size" << endl;
                    continue;
                }

                playerNicks.resize(qsl2.at(0).toInt());

                for (int i = 0; i < playerNicks.size(); ++i)
                    playerNicks[i] = qsl2.at(i+1);

                roundCount = qsl.at(4).toInt();

                emit initReceived(planetCount, distanceMatrix, planets, playerNicks, roundCount);
            } break;
            case TURN_DISPLAY:
            {

            } break;
            default:
                cerr << "Unknown message received (" + message.toStdString() + ')' << endl;
            }
        }
    } while (index != -1);
}

void Network::connectToHost(QString address, quint16 port)
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

    QByteArray message;

    message += LOGIN_DISPLAY;
    message += '\n';

    _socket.write(message);
}
