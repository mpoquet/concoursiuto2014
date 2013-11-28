#include "game.hpp"

#include <QDebug>
#include <QFile>
#include <QStringList>
#include <QDateTime>

Game::Game(QString mapFilename, int delayBetweenRound, int roundCount, AbstractGameModel * gameModel)
{
	m_delayBetweenRound = delayBetweenRound;
	m_gameModel = gameModel;
	m_roundCount = roundCount;

	QFile mapFile(mapFilename);

	if(mapFile.open(QIODevice::ReadOnly))
	{
		QByteArray data = mapFile.readAll();
		mapFile.close();

		QStringList lines = QString(data).trimmed().split('\n');
		int lineNumber = 0;
		foreach(QString line, lines)
		{
			QStringList numbers = line.trimmed().split(' ');

			if(numbers.size() == 6)
			{
				int x = numbers[0].toInt();
				int y = numbers[1].toInt();
				int taille = numbers[2].toInt();
				int idGalaxy = numbers[3].toInt();
				bool initial = numbers[4].toInt() == 1;
				bool neutral = numbers[5].toInt() == 1;

				Planet * newPlanet = new Planet(m_planets.size(), idGalaxy, x, y, taille, initial, neutral);
				getGalaxy(idGalaxy)->addPlanet(newPlanet);

				m_planets.push_back(newPlanet);
			}
			else
			{
				qDebug() << "malformed line " << lineNumber << " in map file";
			}
			lineNumber++;
		}

		Planet * pSource;
		Planet * pDest;
		int distance;
		for(int i = 0 ; i < m_planets.size() ; ++i)
		{
			pSource = m_planets[i];
			pSource->addDistance(pSource, 0);
			for(int j = i+1 ; j < m_planets.size() ; ++j)
			{
				pDest = m_planets[j];
				distance = m_gameModel->getDistance(pSource, pDest);
				pSource->addDistance(pDest, distance);
				pDest->addDistance(pSource, distance);
			}
		}
	}
	else
	{
		qDebug() << "Unable to open the map file : " << mapFilename;
        throw new std::exception();
	}
}

void Game::start()
{
	Q_ASSERT(m_players.size() > 1);

	if(m_players.size() <= 1)
	{
        qDebug() << "Game::start() : il faut au moins deux joueurs...";
		return;
	}

    m_playerCount = m_players.size();
	emit gameStarted();

	qDebug() << "Game::start()";

	m_currentRound = 0;
	// attribution des ids aux joueurs de 1 à 4
	// 0 = autochtone
	// -1 = personne
	int id = 1;
	foreach(Player * p, m_players)
	{
		p->setId(id);
		id++;
	}

	// attribution des planètes aux joueurs
	foreach(Galaxy * g, m_galaxies)
	{
		QVector<Planet*> initialPlanets;
		foreach(Planet * p, g->planets())
		{
			if(p->isInitial())
			{
				initialPlanets.append(p);
			}
		}

		foreach(Player * p, m_players)
		{
			int planetIndex = qrand() % initialPlanets.size();
			Planet * planet = initialPlanets[planetIndex];

			planet->setOwner(p);

			initialPlanets.remove(planetIndex);
		}
	}

	// attribution des planètes autochtones + attribution des vaisseaux sur chaque planète
	m_neutralPlayer = new Player("autochtone");
	m_nullPlayer = new Player("null");
	m_nullPlayer->setId(-1);
	m_neutralPlayer->setId(0);
	foreach(Planet * p, m_planets)
	{
		if(p->owner() == nullptr)
		{
			if(p->isNeutral())
			{
				p->setOwner(m_neutralPlayer);
				p->setShipCount(m_gameModel->getSpaceShipForNeutral(p->size()));
			}
			else
			{
				p->setOwner(m_nullPlayer);
				p->setShipCount(0);
			}
		}
		else
		{
			p->setShipCount(m_gameModel->getSpaceShipForPlayer(p->size()));
		}
	}

	foreach(Player * p, m_players)
	{
		p->setResources(m_gameModel->getInitialResource(p));
	}
/*
	qDebug() << "Nombre planete : " << m_planets.size();
	qDebug() << "nombre de tour : " << m_gameModel->getRoundCount();
	qDebug() << "max scan : " << m_gameModel->getMaxScan();
	qDebug() << "space ship cost : " << m_gameModel->getSpaceShipCost();


	qDebug() << "Matrice de distance : " << getDistanceMatrix();
*/
    QVector<QVector<int> > distanceMatrix = getDistanceMatrix();

	foreach(Player * p, m_players)
	{
		if(p->id() > 0)
		{
			emit initPlayerSignal(
				m_clientSockets.value(p),
				m_planets.size(),
                distanceMatrix,
				m_roundCount,
				m_gameModel->getMaxScan(),
				m_gameModel->getSpaceShipCost(),
				m_players.size(),
				p->id()
			);
		}
    }

    if (!m_displaySockets.empty())
    {
        QVector<InitDisplayPlanet> planets(m_planets.size());
        QVector<QString> playerNicks(m_players.size());

        for (int i = 0; i < planets.size(); ++i)
        {
            planets[i].planetSize = m_planets[i]->size();
            planets[i].playerID = m_planets[i]->owner()->id();
            planets[i].posX = m_planets[i]->x();
            planets[i].posY = m_planets[i]->y();
            planets[i].shipCount = m_planets[i]->shipCount();
        }

        for (int i = 0; i < playerNicks.size(); ++i)
            playerNicks[i] = m_players[i]->nickname();

        for (int i = 0; i < m_displaySockets.size(); ++i)
            emit initDisplaySignal(m_displaySockets[i],
                                   planets.size(),
                                   distanceMatrix,
                                   planets,
                                   playerNicks,
                                   m_roundCount);
    }

	m_timer = new QTimer();
	m_timer->setInterval(m_delayBetweenRound);

	connect(m_timer, SIGNAL(timeout()), this, SLOT(iteration()));

	m_timer->start();
}

void Game::iteration()
{
    if(m_currentRound >= m_roundCount || (m_playerCount == 0 && m_movements.empty()))
    {
        m_timer->stop();

        // todo : handle score
        foreach(Player * p, m_players)
        {
            emit finishedSignal(m_clientSockets.value(p), true);
        }

		return;
	}

    emit roundStarted(m_currentRound);

	m_currentRound++;

    qDebug() << '\n' << QDateTime::currentDateTime().toString("[hh:mm:ss]") << "Game::iteration" << m_currentRound;
    /*if (m_playerCount == 0)
    {
        int a = -1;
        int b = 40000000;

        for (int i = 0; i < m_movements.size(); ++i)
        {
            a = std::max(m_movements[i]->remainingRound, a);
            b = std::min(m_movements[i]->remainingRound, b);
        }

        qDebug() << QString("(MoveCount, MaxRRound, MinRR) = (%1,%2,%3)").arg(m_movements.size()).arg(a).arg(b);
    }*/

	Planet * planet;
	Planet * planetDest;

	QVector<ShipMovement*> newMovements;
	QVector<ShipMovement*> endMovements;

	// space ship creation + fleet launch
	foreach(Player * p, m_players)
	{
		foreach(BuildOrder b, p->waitingBuild())
		{
			int totalCost = m_gameModel->getSpaceShipCost() * b.shipCount;
			if(p->resources() >= totalCost)
			{
				planet = getPlanet(b.planet);
				if(planet != nullptr && planet->owner() == p)
				{
					planet->setShipCount(planet->shipCount() + b.shipCount);
					p->setResources(p->resources() - totalCost);
				}
			}
		}

		foreach(ShipMove m, p->waitingMove())
		{
			planet = getPlanet(m.srcPlanet);
			planetDest = getPlanet(m.destPlanet);
			if(planet != nullptr && planetDest != nullptr && planet->owner() == p)
			{
				if(planet->shipCount() <= m.shipCount)
				{
					m.shipCount = planet->shipCount()-1;
				}
				planet->setShipCount(planet->shipCount() - m.shipCount);

				ShipMovement * move = new ShipMovement();
				move->move = m;
				move->remainingRound = planet->distance(planetDest);
				move->player = p->id();

				newMovements.append(move);
			}
		}
	}

    // Let's clean the waitingBuilds and waitingMoves for each player
    foreach (Player * p, m_players)
    {
        p->clearOrders();
    }

	for(int i = 0 ; i < m_movements.size() ; ++i)
	{
		m_movements[i]->remainingRound--;
        if(m_movements[i]->remainingRound <= 0)
		{
			endMovements.append(m_movements[i]);
			m_movements.remove(i);
			i--;
		}
	}

	// ajout des nouvelles flottes dans la liste de déplacement.
	m_movements += newMovements;

	QMap<int, QVector<FightReport> > reports = handleBattle(endMovements);

	foreach(Player * p, m_players)
	{
		int resourceInc = 0;
		foreach(Planet * pl, p->planets())
		{
			resourceInc += m_gameModel->getResourcesByRound(pl->size());
		}
		p->setResources(p->resources() + resourceInc);
	}

	// check if someone lost or won
	if(m_players.size() > 1)
	{
		for(int i = 0 ; i < m_players.size() ; ++i)
		{
			Player * p = m_players[i];
			//no more planets
			if(p->planets().empty() && !hasFleet(p))
			{
				qDebug() << "Player with id : " << p->id() << " lost !";
				emit finishedSignal(m_clientSockets.value(p), false);
				m_players.remove(i);
				i--;
			}
		}
	}
	if(m_players.size() == 1)
	{
		Player * winner = m_players[0];
		qDebug() << "Player with id : " << winner->id() << " won !";
		emit finishedSignal(m_clientSockets.value(winner), true);
		m_players.clear();
		m_timer->stop();
	}
	else if(m_players.size() == 0)
	{
		m_timer->stop();
	}

	sendTurnMessage(reports);
}

void Game::playerLogin(QTcpSocket *socket, QString nickname)
{
	Player * p = new Player(nickname);
	m_players.push_back(p);
	m_clientSockets.insert(p, socket);
}

void Game::playerOrder(QTcpSocket *socket, QVector<int> planetsToScan, QVector<BuildOrder> shipsToBuild, QVector<ShipMove> shipsToMove)
{
	Player * p = m_clientSockets.key(socket);

	filterBuildOrder(shipsToBuild, p);
	filterShipMove(shipsToMove, p);
	filterScan(planetsToScan);

    /*qDebug() << "Order received from player " << p->id() << " (" << p->nickname() << ")";
	qDebug() << "===> Spaceship build";
	foreach(BuildOrder b, shipsToBuild)
	{
		qDebug() << "Planet : " << b.planet << " numbers : " << b.shipCount;
	}
	qDebug() << "===> Move";
	foreach(ShipMove m, shipsToMove)
	{
		qDebug() << "From " << m.srcPlanet << " To " << m.destPlanet << " with " << m.shipCount;
    }*/

	p->setBuildOrder(shipsToBuild);
	p->setShipMove(shipsToMove);
	QVector<Planet*> scan;
	Planet * planet;
	foreach(int idPlanet, planetsToScan)
	{
		planet = getPlanet(idPlanet);
		if(planet != nullptr)
		{
			scan.push_back(planet);
		}
	}
	p->setPlanetScan(scan);
}

void Game::displayLogin(QTcpSocket *socket)
{
    m_displaySockets.append(socket);
}

Planet * Game::getPlanet(int id)
{
	foreach(Planet * p, m_planets)
	{
		if(p->id() == id)
			return p;
	}
	return nullptr;
}

Galaxy * Game::getGalaxy(int id)
{
	foreach(Galaxy * g, m_galaxies)
	{
		if(g->id() == id)
			return g;
	}
	Galaxy * g = new Galaxy(id);
	m_galaxies.append(g);
	return g;
}

Player * Game::getPlayer(int id)
{
	if(id == 0)
		return m_neutralPlayer;
	if(id == -1)
		return m_nullPlayer;
	foreach(Player * p, m_players)
	{
		if(p->id() == id)
			return p;
	}
	return nullptr;
}

void Game::filterBuildOrder(QVector<BuildOrder> & orders, Player * player)
{
	int planetId;
	int maxBuild;
	Planet * planet;
	bool ok = false;
	int usedResources = 0;
	int availableResources;
	int spaceShipCost = m_gameModel->getSpaceShipCost();

	for(int i = 0 ; i < orders.size() ; ++i)
	{
		ok = false;
		planetId = orders[i].planet;
		planet = getPlanet(planetId);
		if(planet != nullptr)
		{
			if(planet->owner() == player)
			{
				ok = true;
				maxBuild = m_gameModel->getMaxBuildByRound(planet->size());
				if(orders[i].shipCount > maxBuild)
				{
					orders[i].shipCount = maxBuild;
				}

				if(usedResources + orders[i].shipCount * spaceShipCost > player->resources())
				{
					if(usedResources + spaceShipCost > player->resources())
					{
						orders.resize(i);
						break;
					}
					else
					{
						availableResources = player->resources() - usedResources;
						orders[i].shipCount = availableResources / spaceShipCost;
						orders.resize(i+1);
						break;
					}
				}

				usedResources += orders[i].shipCount * spaceShipCost;
			}
		}

		if(!ok)
		{
			orders.remove(i);
			i--;
		}
	}
}

void Game::filterShipMove(QVector<ShipMove> & shipMoves, Player * player)
{
	int idSource;
	int idDest;
    Planet * planetSource;
    Planet * planetDest;
	bool ok = false;
	for(int i = 0 ; i < shipMoves.size() ; ++i)
	{
		ok = false;
		idSource = shipMoves[i].srcPlanet;
		idDest = shipMoves[i].destPlanet;

        planetSource = getPlanet(idSource);
        planetDest = getPlanet(idDest);

        if(planetSource != nullptr && planetDest != nullptr && planetSource->owner() == player && planetSource != planetDest)
		{
            if(planetSource->shipCount() > 1)
			{
				ok = true;
                if(planetSource->shipCount() < shipMoves[i].shipCount)
				{
                    shipMoves[i].shipCount = planetSource->shipCount() - 1;
				}
			}
		}

		if(!ok)
		{
			shipMoves.remove(i);
			i--;
		}
	}
}

void Game::filterScan(QVector<int> & scans)
{
	for(int i = 0 ; i < scans.size() ; ++i)
	{
		if(scans[i] < 0 || scans[i] >= m_planets.size())
		{
			scans.remove(i);
			i--;
		}
	}
	int max = m_gameModel->getMaxScan();
	if(scans.size() > max)
	{
		scans.resize(max);
	}
}

QVector<QVector<int> > Game::getDistanceMatrix()
{
	QVector<QVector<int> > matrix(m_planets.size(), QVector<int>(m_planets.size(), 0));

	foreach(Planet * source, m_planets)
	{
		foreach(Planet * dest, m_planets)
		{
			matrix[source->id()][dest->id()] = source->distance(dest);
		}
	}
	return matrix;
}

void Game::sendTurnMessage(QMap<int, QVector<FightReport> > reports)
{
	foreach(Player * p, m_players)
	{
		QVector<OurShipsOnPlanets> ourShipsOnPlanets;
		QVector<ScanResult> scanResults;
		QVector<OurMovingShips> ourMovingShips;
		QVector<IncomingEnnemyShips> incomingEnnemies;


		//scans
		foreach(Planet * pl, p->waitingScan())
		{
			if(pl != nullptr)
			{
				ScanResult scan;
				scan.planet = pl->id();
				scan.player = pl->owner()->id();
				scan.resourcePerRound = m_gameModel->getResourcesByRound(pl->size());
				scan.maxBuildPerRound = m_gameModel->getMaxBuildByRound(pl->size());
				scan.shipCount = pl->shipCount();
				scanResults.append(scan);
			}
		}

		//vaisseaux sur nos planete
		foreach(Planet * pl, p->planets())
		{
			OurShipsOnPlanets ships;
			ships.planet = pl->id();
			ships.maxBuildPerRound = m_gameModel->getMaxBuildByRound(pl->size());
			ships.resourcePerRound = m_gameModel->getResourcesByRound(pl->size());
			ships.shipCount = pl->shipCount();
			ourShipsOnPlanets.append(ships);
		}

		//etat de nos flottes + arrivant vers nous
		foreach(ShipMovement * m, m_movements)
		{
			if(m->player == p->id())
			{
				OurMovingShips move;
				move.srcPlanet = m->move.srcPlanet;
				move.destPlanet = m->move.destPlanet;
				move.remainingTurns = m->remainingRound;
				move.shipCount = m->move.shipCount;
				ourMovingShips.append(move);
			}
			else
			{
				Planet * pl = getPlanet(m->move.destPlanet);
				if(pl != nullptr && pl->owner() == p)
				{
					IncomingEnnemyShips ennemy;
					ennemy.srcPlanet = m->move.srcPlanet;
					ennemy.destPlanet = m->move.destPlanet;
					ennemy.shipCount = m->move.shipCount;
					incomingEnnemies.append(ennemy);
				}
			}
		}

        emit turnPlayerSignal(m_clientSockets.value(p),
						m_currentRound,
						p->resources(),
						ourShipsOnPlanets,
						scanResults,
						ourMovingShips,
						incomingEnnemies,
						reports[p->id()]);
	}

    // Let's send the turn to every observer
    if (!m_displaySockets.empty())
    {
        QVector<int> scores(m_players.size());
        QVector<TurnDisplayPlanet> planets(m_planets.size());
        QVector<ShipMovement> movements(m_movements.size());

        for (int i = 0; i < scores.size(); ++i)
            scores[i] = 42; // todo : handle score

        for (int i = 0; i < planets.size(); ++i)
            planets[i] = {m_planets[i]->owner()->id(), m_planets[i]->shipCount()};

        for (int i = 0; i < movements.size(); ++i)
            movements[i] = *m_movements[i];

        for (int i = 0; i < m_displaySockets.size(); ++i)
            emit turnDisplaySignal(m_displaySockets[i], scores, planets, movements);
    }
}

bool Game::hasFleet(Player * p)
{
	foreach(ShipMovement * m, m_movements)
	{
		if(m->player == p->id())
		{
			return true;
		}
	}
	return false;
}

QMap<int, QVector<FightReport> > Game::handleBattle(QVector<ShipMovement*> endMovements)
{
	//  <planetID, <playerId, ship count>
	QMap<int, QMap<int, int> > m_incomingFleet;
	QMap<int, QVector<FightReport> > reports;
	Planet * planet;
	QVector<Fleet> fleets;

	foreach(ShipMovement * m, endMovements)
	{
		if(m_incomingFleet[m->move.destPlanet].contains(m->player))
		{
			m_incomingFleet[m->move.destPlanet][m->player] += m->move.shipCount;
		}
		else
		{
			m_incomingFleet[m->move.destPlanet][m->player] = m->move.shipCount;
		}
	}

	foreach(int planetId, m_incomingFleet.keys())
	{
		planet = getPlanet(planetId);

		if(planet != nullptr)
		{
			//résolution du combat sur la planete
			fleets.clear();

			Fleet planetFleet;
			planetFleet.player = planet->owner()->id();
			planetFleet.shipCount = planet->shipCount();
			bool fleetAppend = false;

			foreach(int playerId, m_incomingFleet[planetId].keys())
			{
				Fleet f;
				f.player = playerId;
				f.shipCount = m_incomingFleet[planetId][playerId];
				if(f.player == planetFleet.player)
				{
					f.shipCount += planetFleet.shipCount;
					fleetAppend = true;
				}
				fleets.append(f);
			}
			if(!fleetAppend)
			{
				fleets.append(planetFleet);
			}

			//DEBUG
            /*qDebug() << "Battle resolution on planet : " << planet->id() << " owner : " << planet->owner()->id();
			qDebug() << "Fleets : ";
			foreach(Fleet f, fleets)
			{
				qDebug() << "owner : " << f.player << " : " << f.shipCount;
            }*/

			//////

			Fleet winner = m_gameModel->resolveBattle(fleets);
            //qDebug() << "battle winner : " << winner.player << " : " << winner.shipCount;


			FightReport report;
			report.planet = planetId;
			report.playerCount = m_incomingFleet[planetId].keys().size();
			report.aliveShipCount = winner.shipCount;
			report.winner = winner.player;

			foreach(int playerId, m_incomingFleet[planetId].keys())
			{
				reports[playerId].append(report);
			}

			if(planet->owner()->id() != winner.player)
			{
				Player * p = getPlayer(winner.player);

				Q_ASSERT(p != nullptr);

				planet->setOwner(p);
			}
			planet->setShipCount(winner.shipCount);
		}
	}

	return reports;
}

void Game::playerDisconnected(QTcpSocket * socket)
{
    (void) socket;
    --m_playerCount;

    /*
     * Le code ci-dessous est commenté car il fait planter les
     * combats suivants (lors de l'assertion p != nullptr)
     * avec le joueur déconnecté
     *
     * Puisque le code est commenté, le jeu ne sait pas qu'il n'y a plus qu'un joueur vivant
     *

    Player * p = m_clientSockets.key(socket);
	for(int i = 0 ; i < m_players.size() ; ++i)
	{
		if(p == m_players[i])
		{
			m_players.remove(i);
            return;
		}
    }*/
}

void Game::displayDisconnected(QTcpSocket *socket)
{
    int i = m_displaySockets.indexOf(socket);

    if (i != -1)
        m_displaySockets.remove(i);
}

void Game::setDelayBetweenRound(int ms)
{
	m_delayBetweenRound	= ms;
}

void Game::setNumberOfRound(int round)
{
	m_roundCount = round;
}
