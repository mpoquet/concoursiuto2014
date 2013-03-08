#include "galaxy.hpp"

Galaxy::Galaxy(int id)
{
	m_id = id;
}

int Galaxy::id()
{
	return m_id;
}

void Galaxy::setId(int id)
{
	m_id = id;
}

void Galaxy::addPlanet(Planet * planet)
{
	m_planets.append(planet);
}

QVector<Planet*> Galaxy::planets()
{
	return m_planets;
}
