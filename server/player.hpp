#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "struct.hpp"
#include "planet.hpp"

#include <QVector>
#include <QString>

class Player
{
	public:
		Player(QString nickname);
		~Player();

		void setId(int id);
		void setShipMove(QVector<ShipMove> shipMoves);
		void setBuildOrder(QVector<BuildOrder> buildOrders);
		void setPlanetScan(QVector<Planet*> planetScan);
		void setResources(int resources);

		void addPlanet(Planet* planet);
		void removePlanet(Planet* planet);
		QVector<Planet*> planets();

		int resources();
		int id();
		QString nickname();

		QVector<Planet*> waitingScan();
		QVector<ShipMove> waitingMove();
		QVector<BuildOrder> waitingBuild();

	protected:
		QString m_nickname;
		int m_id;
		int m_resources;

		QVector<Planet*> m_planets;

		QVector<Planet*> m_waitingPlanetScan;
		QVector<ShipMove> m_waitingShipMoves;
		QVector<BuildOrder> m_waitingBuildOrders;
};

#endif // PLAYER_HPP
