#ifndef PLANET_HPP
#define PLANET_HPP

class Player;

#include <QMap>
#include <QPair>

class Planet
{
	public:
		Planet(int id, int idGalaxy, int x, int y, int size, bool isInitial, bool isNeutral);
		~Planet();

		int id();
		int idGalaxy();
		int size();
		int x();
		int y();
		bool isInitial();
		bool isNeutral();

		int shipCount();
		Player * owner();
		int distance(Planet * dest);

		void setOwner(Player * player);
		void setShipCount(int shipCount);

		void addDistance(Planet * dest, int distance);


	private:
		int m_id;
		int m_idGalaxy;
		bool m_isInitial;
		bool m_isNeutral;
		int m_xPos;
		int m_yPos;
		int m_size;
		Player * m_owner;
		int m_shipCount;
		QMap<Planet *, int> m_distance;
};

#endif // PLANET_HPP
