#ifndef GALAXY_HPP
#define GALAXY_HPP

#include "planet.hpp"

#include <QVector>

class Galaxy
{
	public:
		Galaxy(int id);

		int id();
		void setId(int id);

		void addPlanet(Planet * planets);
		QVector<Planet*> planets();

	private:
		int m_id;
		QVector<Planet*> m_planets;
};

#endif // GALAXY_HPP
