#ifndef STRUCT_HPP
#define STRUCT_HPP

struct BuildOrder
{
	int planet;
	int shipCount;
};

struct ShipMove
{
	int srcPlanet;
	int destPlanet;
	int shipCount;
};

struct ShipMovement
{
	ShipMove move;
	int remainingRound;
};

struct OurShipsOnPlanets
{
	int planet;
	int planetSize;
	int shipCount;
};

struct ScanResult
{
	int planet;
	int player;
	int shipCount;
	int resource;
	int shipMaxBuild;
};

struct OurMovingShips
{
	int srcPlanet;
	int destPlanet;
	int shipCount;
	int remainingTurns;
};

struct IncomingEnnemyShips
{
	int srcPlanet;
	int destPlanet;
	int shipCount;
};

struct FightReport
{
	int planet;
	int winner;
	int playerCount;
	int aliveShipCount;
};


#endif // STRUCT_HPP
