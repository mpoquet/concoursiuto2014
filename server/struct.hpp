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


#endif // STRUCT_HPP
