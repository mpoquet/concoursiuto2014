#include "player.hpp"

Player::Player(QString nickname)
{
	m_nickname = nickname;
	m_id = 0;
	m_resources = 0;
    m_score = 0;
}

Player::~Player()
{

}

void Player::setId(int id)
{
	m_id = id;
}

void Player::setShipMove(QVector<ShipMove> shipMoves)
{
	m_waitingShipMoves = shipMoves;
}

void Player::setBuildOrder(QVector<BuildOrder> buildOrders)
{
	m_waitingBuildOrders = buildOrders;
}

void Player::setPlanetScan(QVector<Planet*> planetScan)
{
	m_waitingPlanetScan = planetScan;
}

void Player::setResources(int resources)
{
    m_resources = resources;
}

void Player::setScore(int score)
{
    m_score = score;
}

void Player::clearOrders()
{
    m_waitingShipMoves.clear();
    m_waitingBuildOrders.clear();
    m_waitingPlanetScan.clear();
}

void Player::addPlanet(Planet *planet)
{
	m_planets.append(planet);
}

void Player::removePlanet(Planet *planet)
{
	for(int i = 0 ; i < m_planets.size() ; ++i)
	{
		if(planet == m_planets[i])
		{
			m_planets.remove(i);
			return;
		}
	}
}

QVector<Planet*> Player::planets()
{
	return m_planets;
}

int Player::resources()
{
	return m_resources;
}

int Player::id()
{
	return m_id;
}

QString Player::nickname()
{
    return m_nickname;
}

int Player::score()
{
    return m_score;
}

QVector<Planet*> Player::waitingScan()
{
	return m_waitingPlanetScan;
}

QVector<ShipMove> Player::waitingMove()
{
	return m_waitingShipMoves;
}

QVector<BuildOrder> Player::waitingBuild()
{
	return m_waitingBuildOrders;
}
