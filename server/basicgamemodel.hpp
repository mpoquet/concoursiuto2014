#ifndef BASICGAMEMODEL_HPP
#define BASICGAMEMODEL_HPP

#include "abstractgamemodel.hpp"

class BasicGameModel : public AbstractGameModel
{
	public:
		BasicGameModel();

		virtual int getSpaceShipForPlayer(int planetSize);
		virtual int getSpaceShipForNeutral(int planetSize);
		virtual int getInitialResource(Player * player);

		virtual int getMaxBuildByRound(int planetSize);
		virtual int getResourcesByRound(int planetSize);

		virtual int getMaxScan();

		virtual int getSpaceShipCost();

		virtual int getDistance(Planet * p1, Planet * p2);

		virtual Fleet resolveBattle(QVector<Fleet> fleets);

	protected:
		Fleet simulateBattle(Fleet f1, Fleet f2);
};

#endif // BASICGAMEMODEL_HPP
