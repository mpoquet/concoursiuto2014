#include "planet.hpp"

#include "player.hpp"

#include <QtCore/QDebug>

Planet::Planet(int id, int idGalaxy, int x, int y, int size, bool isInitial, bool isNeutral, int autochtoneNumber)
{
	m_id = id;
	m_idGalaxy = idGalaxy;
	m_xPos = x;
	m_yPos = y;
	m_size = size;
	m_isInitial = isInitial;
	m_isNeutral = isNeutral;

	m_owner = nullptr;
	m_shipCount = 0;
	m_doubleSpeed = 0;

	m_autochtoneNumber = autochtoneNumber;
}

Planet::~Planet()
{

}

int Planet::autochtoneNumber()
{
	return m_autochtoneNumber;
}

int Planet::id()
{
	return m_id;
}

int Planet::idGalaxy()
{
	return m_idGalaxy;
}

int Planet::size()
{
	return m_size;
}

int Planet::x()
{
	return m_xPos;
}

int Planet::y()
{
	return m_yPos;
}

bool Planet::isInitial()
{
	return m_isInitial;
}

bool Planet::isNeutral()
{
	return m_isNeutral;
}

int Planet::shipCount()
{
	return m_shipCount;
}

int Planet::doubleSpeed()
{
	return m_doubleSpeed;
}

Player * Planet::owner()
{
	return m_owner;
}

int Planet::distance(Planet * dest)
{
	return m_distance[dest];
}

void Planet::setShipCount(int shipCount)
{
	m_shipCount = shipCount;
}

void Planet::setDoubleSpeed(int turns)
{
	m_doubleSpeed = turns;
}

void Planet::addDistance(Planet * dest, int distance)
{
	m_distance.insert(dest, distance);
}

void Planet::setOwner(Player * player)
{
	if(this->m_owner != nullptr)
		this->m_owner->removePlanet(this);
	player->addPlanet(this);
	m_owner = player;

	if(player->id() < 0) {
		qDebug() << "\t\tERROR => setOwner empty";
	}
}
