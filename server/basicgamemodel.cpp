#include "basicgamemodel.hpp"

BasicGameModel::BasicGameModel()
{
}

int BasicGameModel::getSpaceShipForPlayer(int planetSize)
{
	return planetSize * 10;
}

int BasicGameModel::getSpaceShipForNeutral(int planetSize)
{
	return planetSize * 2;
}

int BasicGameModel::getInitialResource(Player * player)
{
	Q_UNUSED(player);
	return 100;
}

int BasicGameModel::getMaxBuildByRound(int planetSize)
{
	return planetSize * 10;
}

int BasicGameModel::getResourcesByRound(int planetSize)
{
	return planetSize * 3;
}

int BasicGameModel::getMaxScan()
{
	return 10;
}

int BasicGameModel::getSpaceShipCost()
{
	return 5;
}

int BasicGameModel::getDistance(Planet * p1, Planet * p2)
{
	//calcul de distance d'euclide + arrondi pour le nombre de tour : 5 mètre = 1 tour
	int x = abs(p1->x() - p2->x());
	int y = abs(p1->y() - p2->y());

	/*
	qDebug() << "Distance " << p1->id() << "(" << p1->x() << " ; " << p1->y() << ")" << " ; " << p2->id() <<
				 "(" << p2->x() << " ; " << p2->y() << ")" << " x = " << x << " y = " << y;
	*/
	int distance = (int)round(sqrt(x*x+y*y) / 5);
	return distance;
}

Fleet BasicGameModel::resolveBattle(QVector<Fleet> fleets)
{
	if(fleets.size() <= 2)
	{
		if(fleets.size() == 0)
		{
			return Fleet(-1, 0);
		}
		else if(fleets.size() == 1)
		{
			return fleets[0];
		}
		else
		{
			return simulateBattle(fleets[0], fleets[1]);
		}
	}
	//division de chaque flotte pas fleets.size() - 1
	QVector<QVector<Fleet> > fleetsDivision;

	//division de la flotte sous forme de matrice
	for(int i = 0 ; i < fleets.size() ; ++i)
	{
		QVector<Fleet> division;
		int dividers = fleets.size() - 1;

		for(int j = 0 ; j < fleets.size() ; ++j)
		{
			if(j == i)
			{
				division.append(Fleet(-1, 0));
			}
			else
			{
				division.append(Fleet(fleets[i].player, fleets[i].shipCount / dividers));
			}
		}

		if(i != fleets.size() - 1)
			division[fleets.size()-1].shipCount += fleets[i].shipCount % dividers;
		else
			division[fleets.size()-2].shipCount += fleets[i].shipCount % dividers;

		fleetsDivision.append(division);
	}

	//resolution des combats 2 a 2
	QMap<int, Fleet> results;
	for(int i = 0 ; i < fleets.size() ; ++i)
	{
		for(int j = i+1 ; j < fleets.size() ; ++j)
		{
			Fleet res = simulateBattle(fleetsDivision[i][j], fleetsDivision[j][i]);
			if(results.contains(res.player))
			{
				results[res.player].shipCount += res.shipCount;
			}
			else
			{
				results[res.player] = res;
			}
		}
	}

	return resolveBattle(results.values().toVector());
}

Fleet BasicGameModel::simulateBattle(Fleet f1, Fleet f2)
{
	if(f2.shipCount > f1.shipCount)
	{
		f2.shipCount -= f1.shipCount;
		return f2;
	}
	else
	{
		f1.shipCount -= f2.shipCount;
		return f1;
	}
}
