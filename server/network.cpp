#include "network.hpp"

#include <iostream>

#include <QStringList>

#include "protocole.h"
#include "game.hpp"

using namespace std;

Network::Network(quint16 port, int maxPlayerCount, int maxDisplayCount, Game *game, QObject *parent) :
	QObject(parent),
	_server(new QTcpServer(this)),
    _port(port),
	_maxPlayerCount(maxPlayerCount),
    _maxDisplayCount(maxDisplayCount),
    _game(game)
{
	if (_maxPlayerCount < 0)
		_maxPlayerCount = 0;

	if (_maxDisplayCount < 0)
		_maxDisplayCount = 0;

	connect(_server, &QTcpServer::newConnection, this, &Network::onNewConnection);

	_regexMessage.setPattern(QString("([%1%2%3])(.*)").arg(LOGIN_PLAYER).arg(LOGIN_DISPLAY).arg(MOVE_PLAYER));
	_regexLoginPlayer.setPattern("[a-zA-Z0-9_\\-]{1,10}");
	_regexMovePlayer.setPattern(QString("(.*)%1(.*)%1(.*)").arg(SEP));
	_regexScans.setPattern(QString("(\\d+)(?:%1\\d+)*").arg(SSEP));
	_regexBuilds.setPattern(QString("(\\d+)(?:%1\\d+%1\\d+)*").arg(SSEP));
	_regexMoves.setPattern(QString("(\\d+)(?:%1\\d+%1\\d+%1\\d+)*").arg(SSEP));

	Q_ASSERT(_regexMessage.isValid());
	Q_ASSERT(_regexLoginPlayer.isValid());
	Q_ASSERT(_regexMovePlayer.isValid());
	Q_ASSERT(_regexScans.isValid());
	Q_ASSERT(_regexBuilds.isValid());
	Q_ASSERT(_regexMoves.isValid());
}

void Network::run()
{
    if (!_server->listen(QHostAddress::Any, _port))
        cerr << "Cannot listen port " << _port << endl;
    else
        cout << "Listening port " << _port << endl;
}

int Network::playerCount() const
{
	int count = 0;

	QMapIterator<QTcpSocket *, Client> it(_clients);
	while (it.hasNext())
	{
		it.next();

		if (it.value().type == Client::PLAYER)
			++count;
	}

	return count;
}

int Network::displayCount() const
{
	int count = 0;

	QMapIterator<QTcpSocket *, Client> it(_clients);
	while (it.hasNext())
	{
		it.next();

		if (it.value().type == Client::DISPLAY)
			++count;
	}

	return count;
}

int Network::clientCount() const
{
	return _clients.size();
}

QVector<QTcpSocket *> Network::clients() const
{
	QVector<QTcpSocket *> v(_clients.size());

	QMapIterator<QTcpSocket *, Client> it(_clients);
	for (int i = 0; it.hasNext(); ++i)
	{
		it.next();
		v[i] = it.key();
	}

	return v;
}

QVector<QTcpSocket *> Network::players() const
{
	QVector<QTcpSocket *> v;

	QMapIterator<QTcpSocket *, Client> it(_clients);
	while (it.hasNext())
	{
		it.next();

		if (it.value().type == Client::PLAYER)
			v.append(it.key());
	}

	return v;
}

QVector<QTcpSocket *> Network::displays() const
{
	QVector<QTcpSocket *> v;

	QMapIterator<QTcpSocket *, Client> it(_clients);
	while (it.hasNext())
	{
		it.next();

		if (it.value().type == Client::DISPLAY)
			v.append(it.key());
	}

    return v;
}

Network::Client::ClientType Network::typeOf(QTcpSocket *socket) const
{
    QMapIterator<QTcpSocket *, Client> it(_clients);
    while (it.hasNext())
    {
        it.next();

        if (it.key() == socket)
            return it.value().type;
    }

    return Client::DISCONNECTED;
}

void Network::sendLoginPlayerACK(QTcpSocket *socket, char value)
{
    _mutexDisconnected.lock();

#ifndef UNIT_TEST
    if (typeOf(socket) == Client::DISCONNECTED)
    {
        qDebug() << "Invalid sendLoginPlayerACK : disconnected socket";
        _mutexDisconnected.unlock();
        return;
    }
#endif

	QByteArray message(3, '\n');
	message[0] = LOGIN_PLAYER_ACK;
	message[1] = value;
	socket->write(message);

    _mutexDisconnected.unlock();
}

void Network::sendLoginDisplayACK(QTcpSocket *socket, char value)
{
    _mutexDisconnected.lock();

#ifndef UNIT_TEST
    if (typeOf(socket) == Client::DISCONNECTED)
    {
        qDebug() << "Invalid sendLoginPlayerACK : disconnected socket";
        _mutexDisconnected.unlock();
        return;
    }
#endif

	QByteArray message(3, '\n');
	message[0] = LOGIN_DISPLAY_ACK;
	message[1] = value;
	socket->write(message);

    _mutexDisconnected.unlock();
}

void Network::sendInitPlayer(QTcpSocket *socket,
							 int planetCount,
							 QVector<QVector<int> > distanceMatrix,
							 int roundCount,
							 int scanLimit, int shipCost, int nbPlayers, int idPlayer)
{
    _mutexDisconnected.lock();

#ifndef UNIT_TEST
    // This is the first sendInitPlayer
    if (_server->isListening())
    {
        // Stop listening pending connections
        _server->close();

        _mutexDisconnected.unlock();

        // Let's close every unlogged socket
        QMapIterator<QTcpSocket*, Client> it(_clients);
        while (it.hasNext())
        {
            it.next();

            if (it.value().type != Client::PLAYER && it.value().type != Client::DISPLAY)
                it.key()->close();
        }

        _mutexDisconnected.lock();
    }

    if (typeOf(socket) == Client::DISCONNECTED)
    {
        qDebug() << "Invalid sendInitPlayer : disconnected socket";
        _mutexDisconnected.unlock();
        return;
    }
#endif

	QByteArray message;

	message += INIT_PLAYER;
	message += QString("%1%2").arg(planetCount).arg(SEP).toLatin1();

    Q_ASSERT(planetCount == distanceMatrix.size());
	for (int y = 0; y < distanceMatrix.size(); ++y)
    {
        Q_ASSERT(planetCount == distanceMatrix[y].size());

        for (int x = 0; x < distanceMatrix[y].size(); ++x)
			message += QString("%1%2").arg(distanceMatrix[y][x]).arg(SSEP).toLatin1();
    }

    // Let's remove the useless SSEP if any
	if (distanceMatrix.size() > 0 && distanceMatrix[0].size() > 0)
		message.chop(1);

	message += SEP;

	message += QString("%1%2").arg(roundCount).arg(SEP).toLatin1();
	message += QString("%1%2").arg(scanLimit).arg(SEP).toLatin1();
	message += QString("%1%2").arg(shipCost).arg(SEP).toLatin1();
	message += QString("%1%2").arg(nbPlayers).arg(SEP).toLatin1();
	message += QString("%1").arg(idPlayer).toLatin1();

	message += '\n';
    socket->write(message);

    _mutexDisconnected.unlock();
}

void Network::sendInitDisplay(QTcpSocket *socket,
                              int planetCount,
                              QVector<QVector<int> > distanceMatrix,
                              QVector<InitDisplayPlanet> planets,
                              QVector<QString> playerNicks,
                              int roundCount)
{
    _mutexDisconnected.lock();

#ifndef UNIT_TEST
    if (typeOf(socket) == Client::DISCONNECTED)
    {
        qDebug() << "Invalid sendInitDisplay : disconnected socket";
        _mutexDisconnected.unlock();
        return;
    }
#endif

    QByteArray message;

    message += INIT_DISPLAY;
    message += QString("%1%2").arg(planetCount).arg(SEP);

    Q_ASSERT(planetCount == distanceMatrix.size());
    for (int y = 0; y < distanceMatrix.size(); ++y)
    {
        Q_ASSERT(planetCount == distanceMatrix[y].size());

        for (int x = 0; x < distanceMatrix[y].size(); ++x)
            message += QString("%1%2").arg(distanceMatrix[y][x]).arg(SSEP).toLatin1();
    }

    // Let's remove the useless SSEP if any
    if (distanceMatrix.size() > 0 && distanceMatrix[0].size() > 0)
        message.chop(1);

    message += SEP;

    for (int i = 0; i < planetCount; ++i)
        message += QString("%1%2%3%2%4%2%5%2%6%2").arg(planets[i].posX).arg(SSEP).arg(
                    planets[i].posY).arg(planets[i].playerID).arg(planets[i].shipCount).arg(
                    planets[i].planetSize).toLatin1();

    // Let's remove the useless SSEP if any
    if (planetCount > 0)
        message.chop(1);

    message += QString("%1%2").arg(SEP).arg(playerNicks.size()).toLatin1();
    for (int i = 0; i < playerNicks.size(); ++i)
        message += QString("%1%2").arg(SSEP).arg(playerNicks[i]).toLatin1();

    message += QString("%1%2").arg(SEP).arg(roundCount);

    message += '\n';
    socket->write(message);

    _mutexDisconnected.unlock();
}

void Network::sendTurnPlayer(QTcpSocket *socket,
    int currentRound,
	int resources,
	QVector<OurShipsOnPlanets> ourShipsOnPlanet,
	QVector<ScanResult> scanResults,
	QVector<OurMovingShips> ourMovingShips,
	QVector<IncomingEnnemyShips> incomingEnnemies,
	QVector<FightReport> fightReports)
{
    _mutexDisconnected.lock();

#ifndef UNIT_TEST
    if (typeOf(socket) == Client::DISCONNECTED)
    {
        qDebug() << "Invalid sendTurnPlayer : disconnected socket";
        _mutexDisconnected.unlock();
        return;
    }
#endif

	QByteArray message;

	message += TURN_PLAYER;
	message += QString("%1%2").arg(currentRound).arg(SEP).toLatin1();
	message += QString("%1%2").arg(resources).arg(SEP).toLatin1();
	message += QString("%1").arg(ourShipsOnPlanet.size()).toLatin1();

	for (int i = 0; i < ourShipsOnPlanet.size(); ++i)
		message += QString("%1%2%1%3%1%4%1%5").arg(SSEP).arg(ourShipsOnPlanet[i].planet).arg(
					ourShipsOnPlanet[i].resourcePerRound).arg(ourShipsOnPlanet[i].maxBuildPerRound).arg(ourShipsOnPlanet[i].shipCount).toLatin1();

	message += QString("%1%2").arg(SEP).arg(scanResults.size()).toLatin1();

	for (int i = 0; i < scanResults.size(); ++i)
		message += QString("%1%2%1%3%1%4%1%5%1%6").arg(SSEP).arg(scanResults[i].planet).arg(
					scanResults[i].player).arg(scanResults[i].resourcePerRound).arg(scanResults[i].maxBuildPerRound).arg(
					scanResults[i].shipCount).toLatin1();

	message += QString("%1%2").arg(SEP).arg(ourMovingShips.size()).toLatin1();

	for (int i = 0; i < ourMovingShips.size(); ++i)
		message += QString("%1%2%1%3%1%4%1%5").arg(SSEP).arg(ourMovingShips[i].srcPlanet).arg(
					ourMovingShips[i].destPlanet).arg(ourMovingShips[i].shipCount).arg(
					ourMovingShips[i].remainingTurns).toLatin1();

	message += QString("%1%2").arg(SEP).arg(incomingEnnemies.size()).toLatin1();

	for (int i = 0; i < incomingEnnemies.size(); ++i)
		message += QString("%1%2%1%3%1%4%1%5").arg(SSEP).arg(incomingEnnemies[i].srcPlanet).arg(
					incomingEnnemies[i].destPlanet).arg(incomingEnnemies[i].shipCount).arg(
					incomingEnnemies[i].remainingTurns).toLatin1();

	message += QString("%1%2").arg(SEP).arg(fightReports.size()).toLatin1();

	for (int i = 0; i < fightReports.size(); ++i)
		message += QString("%1%2%1%3%1%4%1%5").arg(SSEP).arg(fightReports[i].planet).arg(
					fightReports[i].winner).arg(fightReports[i].playerCount).arg(
					fightReports[i].aliveShipCount).toLatin1();

	message += '\n';
    socket->write(message);

    _mutexDisconnected.unlock();
}

void Network::sendTurnDisplay(QTcpSocket *socket,
                              QVector<int> scores,
                              QVector<TurnDisplayPlanet> planets,
                              QVector<ShipMovement> movements)
{
    _mutexDisconnected.lock();

#ifndef UNIT_TEST
    if (typeOf(socket) == Client::DISCONNECTED)
    {
        qDebug() << "Invalid sendTurnDisplay : disconnected socket";
        _mutexDisconnected.unlock();
        return;
    }
#endif

    QByteArray message;

    message += TURN_DISPLAY;

    message += QString("%1").arg(scores.size()).toLatin1();
    for (int i = 0; i < scores.size(); ++i)
        message += QString("%1%2").arg(SSEP).arg(scores[i]).toLatin1();

    message += QString("%1%2").arg(SEP).arg(planets.size()).toLatin1();
    for (int i = 0; i < planets.size(); ++i)
        message += QString("%1%2%1%3").arg(SSEP).arg(planets[i].playerID).arg(planets[i].shipCount).toLatin1();

    message += QString("%1%2").arg(SEP).arg(movements.size()).toLatin1();
    for (int i = 0; i < movements.size(); ++i)
        message += QString("%1%2%1%3%1%4%1%5%1%6").arg(SSEP).arg(movements[i].player).arg(
                    movements[i].move.srcPlanet).arg(movements[i].move.destPlanet).arg(
                    movements[i].move.shipCount).arg(movements[i].remainingRound).toLatin1();

    message += '\n';
    socket->write(message);

    _mutexDisconnected.unlock();
}

void Network::sendFinishedPlayer(QTcpSocket *socket, bool youWon)
{
    _mutexDisconnected.lock();

#ifndef UNIT_TEST
    if (typeOf(socket) == Client::DISCONNECTED)
    {
        qDebug() << "Invalid sendLoginPlayerACK : disconnected socket";
        _mutexDisconnected.unlock();
        return;
    }
#endif

	QByteArray message(3, '\n');
	message[0] = END_OF_GAME;

	if (youWon)
		message[1] = '1';
	else
		message[1] = '0';

	socket->write(message);

    _mutexDisconnected.unlock();
}

void Network::onNewConnection()
{
	QTcpSocket * socket = _server->nextPendingConnection();

    //cout << QString("New connection from %1:%2").arg(socket->peerAddress().toString()).arg(socket->peerPort()).toStdString() << endl;

    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(onMessageReceived()));

    _clients[socket] = Client();

    emit clientConnected(socket);
}

void Network::onMessageReceived()
{
	QTcpSocket * socket = (QTcpSocket *) sender();
	int index;

	_clients[socket].buffer += socket->readAll();

	do
	{
		index = _clients[socket].buffer.indexOf('\n');

		if (index != -1)
		{
			QString message = _clients[socket].buffer.left(index);
			_clients[socket].buffer.remove(0, index+1);
            _clients[socket].receivedMessageCount++;

            // If the server is attached to a game, let's check if the received message count is correct
            if (_game != 0)
            {
                if (_clients[socket].receivedMessageCount > (_game->currentRound() * 2 + 3))
                {
                    cerr << "A client is sending way too many messages. This client is about to be kicked" << endl;
                    socket->close();

                    return;
                }
            }

			if (_regexMessage.exactMatch(message))
			{
				char type = _regexMessage.cap(1).at(0).toLatin1();
				QString content = _regexMessage.cap(2);

				switch (type)
				{
				case LOGIN_PLAYER:
				{
					if (_clients[socket].type == Client::UNKNOWN)
					{
						if (playerCount() < _maxPlayerCount)
						{
							if (_regexLoginPlayer.exactMatch(content))
							{
								_clients[socket].type = Client::PLAYER;

								emit loginPlayer(socket, content);
								sendLoginPlayerACK(socket, OK);
							}
							else
								sendLoginPlayerACK(socket, INVALID_NICKNAME);
						}
						else
							sendLoginPlayerACK(socket, NO_MORE_ROOM);
					}
					else
						sendLoginPlayerACK(socket, ALREADY_LOGGED);

				} break;

				case LOGIN_DISPLAY:
				{
					if (_clients[socket].type == Client::UNKNOWN)
					{
						if (displayCount() < _maxDisplayCount)
						{
							_clients[socket].type = Client::DISPLAY;

							emit loginDisplay(socket);
							sendLoginDisplayACK(socket, OK);
						}
						else
							sendLoginDisplayACK(socket, NO_MORE_ROOM);
					}
					else
						sendLoginDisplayACK(socket, ALREADY_LOGGED);
				} break;

				case MOVE_PLAYER:
				{
					if (_clients[socket].type == Client::PLAYER)
					{
						if (_regexMovePlayer.exactMatch(content))
						{
							QString scans = _regexMovePlayer.cap(1);
							QString builds = _regexMovePlayer.cap(2);
							QString moves = _regexMovePlayer.cap(3);

							QVector<int> planetsToScan;
							QVector<BuildOrder> shipsToBuild;
							QVector<ShipMove> shipsToMove;

							if (_regexScans.exactMatch(scans))
							{
								int scanCount = _regexScans.cap(1).toInt();

								if (scanCount < 0)
									scanCount = 0;

								QStringList qsl = scans.split(SSEP);
								int realScanCount = qsl.size() - 1;

								if (scanCount == realScanCount)
								{
									planetsToScan.resize(scanCount);

									for (int i = 0; i < scanCount; ++i)
										planetsToScan[i] = qsl[i+1].toInt();

									if (_regexBuilds.exactMatch(builds))
									{
										int buildCount = _regexBuilds.cap(1).toInt();

										if (buildCount < 0)
											buildCount = 0;

										qsl = builds.split(SSEP);
										int realBuildCount = (qsl.size() - 1) / 2;
										int remainder = (qsl.size() - 1) % 2;

										if (buildCount == realBuildCount && remainder == 0)
										{
											shipsToBuild.resize(buildCount);

											for (int i = 0; i < buildCount; ++i)
											{
												shipsToBuild[i].planet = qsl[1 + 2*i].toInt();
												shipsToBuild[i].shipCount = qsl[1 + 2*i + 1].toInt();
											}

											if (_regexMoves.exactMatch(moves))
											{
												int moveCount = _regexMoves.cap(1).toInt();

												if (moveCount < 0)
													moveCount = 0;

												qsl = moves.split(SSEP);
												int realMoveCount = (qsl.size() - 1) / 3;
												remainder = (qsl.size() - 1) % 3;

												if (moveCount == realMoveCount && remainder == 0)
												{
													shipsToMove.resize(moveCount);

													for (int i = 0; i < moveCount; ++i)
													{
														shipsToMove[i].srcPlanet = qsl[1 + 3*i].toInt();
														shipsToMove[i].destPlanet = qsl[1 + 3*i + 1].toInt();
														shipsToMove[i].shipCount = qsl[1 + 3*i + 2].toInt();
													}

                                                    //debugDisplayMove(planetsToScan, shipsToBuild, shipsToMove);
													emit movePlayer(socket, planetsToScan, shipsToBuild, shipsToMove);
												}
												else
													cerr << QString("Invalid message : moves count mismatch in move (%1)").arg(moves).toStdString() << endl;
											}
											else
												cerr << QString("Invalid message : invalid moves in move (%1)").arg(moves).toStdString() << endl;
										}
										else
											cerr << QString("Invalid message : build count mismatch in move (%1)").arg(builds).toStdString() << endl;
									}
									else
										cerr << QString("Invalid message : invalid build in move (%1)").arg(builds).toStdString() << endl;
								}
								else
									cerr << QString("Invalid message : scan count mismatch in move (%1)").arg(scans).toStdString() << endl;
							}
							else
								cerr << QString("Invalid message : invalid scan in move (%1)").arg(scans).toStdString() << endl;
						}
						else
							cerr << QString("Invalid message : invalid move (%1)").arg(content).toStdString() << endl;
					}
					else
						cerr << "Invalid message : move from a not-player client" << endl;
				} break;
				}
			}
			else
                cerr << "Invalid message received ('" << message.toStdString() << "')" << endl;
		}
        else if (_clients[socket].buffer.size() > 20000000) // Buffer size > 20 Mo
        {
            cerr << "The buffer of one client exceeded 20 Mo. This client is about to be kicked" << endl;
            _clients[socket].buffer.clear();
            socket->close();

            return;
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
    _mutexDisconnected.lock();

	QTcpSocket * socket = (QTcpSocket *) sender();
    //qDebug() << "Client disconnected";

	Client::ClientType type = _clients[socket].type;

	_clients.remove(socket);
	socket->deleteLater();

	if (type == Client::PLAYER)
		emit playerDisconnected(socket);
	else if (type == Client::DISPLAY)
		emit displayDisconnected(socket);
    else
		emit unloggedClientDisconnected(socket);

    _mutexDisconnected.unlock();
}

void Network::debugDisplayMove(QVector<int> planetsToScan, QVector<BuildOrder> shipsToBuild, QVector<ShipMove> shipsToMove)
{
	cout << "Move:" << endl;
	cout << "  Scans:" << endl;
	for (int i = 0; i < planetsToScan.size(); ++i)
		cout << "    " << planetsToScan[i] << endl;

	cout << "  Build:" << endl;
	for (int i = 0; i < shipsToBuild.size(); ++i)
		cout << "    " << shipsToBuild[i].shipCount << " ships on planet " << shipsToBuild[i].planet << endl;

	cout << "  Moves:" << endl;
	for (int i = 0; i < shipsToMove.size(); ++i)
		cout << "    " << shipsToMove[i].shipCount << " ships from " << shipsToMove[i].srcPlanet << " to " << shipsToMove[i].destPlanet << endl;
	cout << endl;
}
