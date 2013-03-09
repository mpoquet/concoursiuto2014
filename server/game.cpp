#include "game.hpp"

#include <QDebug>
#include <QFile>
#include <QStringList>

Game::Game(QString mapFilename, int delayBetweenRound, AbstractGameModel * gameModel, Network * network)
{
	m_delayBetweenRound = delayBetweenRound;
	m_gameModel = gameModel;
	m_network = network;

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
				pDest = m_planets[i];
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
	Player * neutralPlayer = new Player("autochtone");
	neutralPlayer->setId(0);
	foreach(Planet * p, m_planets)
	{
		if(p->owner() == nullptr)
		{
			if(p->isNeutral())
			{
				p->setOwner(neutralPlayer);
				p->setShipCount(m_gameModel->getSpaceShipForNeutral(p->size()));
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

	foreach(Player * p, m_players)
	{
		if(p->id() > 0)
		{
			emit initPlayerSignal(
				m_clientSockets.value(p),
				m_planets.size(),
				getDistanceMatrix(),
				m_gameModel->getRoundCount(),
				m_gameModel->getMaxScan()
			);
		}
	}
}

void Game::iteration()
{
	//TODO game round iteration implementation

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

				newMovements.append(move);
			}
		}
	}

	for(int i = 0 ; i < m_movements.size() ; ++i)
	{
		m_movements[i]->remainingRound--;
		if(m_movements[i]->remainingRound == 0)
		{
			endMovements.append(m_movements[i]);
			m_movements.remove(i);
			i--;
		}
	}

	//TODO : voir si on le fait avant les déplacements ou non.
	m_movements += newMovements;

	//TODO combat + penser a changer le nombre de vaisseaux restants sur la planete


	foreach(Player * p, m_players)
	{
		int resourceInc = 0;
		foreach(Planet * pl, p->planets())
		{
			resourceInc += m_gameModel->getResourcesByRound(pl->size());
		}
		p->setResources(p->resources() + resourceInc);
	}

	// creation
	// lancement
	// avancement
	// resolution
	//inc resources
	//get scan info

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

	p->setBuildOrder(shipsToBuild);
	p->setShipMove(shipsToMove);
	QVector<Planet*> scan;
	Planet * planet;
	foreach(int idPlanet, planetsToScan)
	{
		planet = getPlanet(idPlanet);
		if(p != nullptr)
		{
			scan.push_back(planet);
		}
	}
	p->setPlanetScan(scan);
}

void Game::displayLogin(QTcpSocket *socket, QString nickname)
{
	Q_UNUSED(socket);
	Q_UNUSED(nickname);
	//TODO implement display relative part.
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
	Planet * planet;
	bool ok = false;
	for(int i = 0 ; i < shipMoves.size() ; ++i)
	{
		ok = true;
		idSource = shipMoves[i].srcPlanet;
		idDest = shipMoves[i].destPlanet;

		planet = getPlanet(idSource);
		if(getPlanet(idDest) != nullptr && planet->owner() == player)
		{
			if(planet->shipCount() > 1)
			{
				ok = true;
				if(planet->shipCount() < shipMoves[i].shipCount)
				{
					shipMoves[i].shipCount = planet->shipCount() - 1;
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
