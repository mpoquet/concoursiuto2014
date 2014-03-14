#ifndef ABSTRACTGAMEMODEL_HPP
#define ABSTRACTGAMEMODEL_HPP

#include "player.hpp"
#include "planet.hpp"
#include "struct.hpp"

#include <QVector>

class AbstractGameModel
{
	public:
		AbstractGameModel();

		virtual int getSpaceShipForPlayer(int planetSize) = 0;
		virtual int getSpaceShipForNeutral(int planetSize) = 0;
		virtual int getInitialResource(Player * player) = 0;

		virtual int getMaxBuildByRound(int planetSize) = 0;
		virtual int getResourcesByRound(int planetSize) = 0;

		virtual int getMaxScan() = 0;

		virtual int getSpaceShipCost() = 0;

		virtual int getDistance(Planet * p1, Planet * p2) = 0;

		virtual Fleet resolveBattle(QVector<Fleet> fleets) = 0;
		
		virtual void handleBonus(Planet* planet, Player* player) = 0;
};

#endif // ABSTRACTGAMEMODEL_HPP
