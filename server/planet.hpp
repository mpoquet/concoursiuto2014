#ifndef PLANET_HPP
#define PLANET_HPP

class Player;

#include <QVector>
#include <QPair>

class Planet
{
	public:
		Planet();
		~Planet();

	private:
		int m_id;
		int m_xPos;
		int m_yPos;
		int m_size;
		Player * m_owner;
		int m_spaceShips;
		QVector<QPair<Planet *, int> > m_distance;
};

#endif // PLANET_HPP
