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
	int player;
	int remainingRound;
};

struct OurShipsOnPlanets
{
	int planet;
	int resourcePerRound;
	int maxBuildPerRound;
	int shipCount;
};

struct ScanResult
{
	int planet;
	int player;
	int shipCount;
	int resourcePerRound;
	int maxBuildPerRound;
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

struct Fleet
{
	int player;
	int shipCount;

	Fleet()
	{
		player = -1;
		shipCount = 0;
	}

	Fleet(int _player, int _shipCount)
	{
		player = _player;
		shipCount = _shipCount;
	}
};


#endif // STRUCT_HPP
