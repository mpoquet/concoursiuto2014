#ifndef STRUCT_HPP
#define STRUCT_HPP

#include <QMetaType>

// To test several things, types must be declarated as a metatype
// To do so, these structs need :
//    - A public default constructor
//    - A public copy constructor
//    - A public destructor

// These strucs also needs to be compared, then the operator==() is implemented

struct BuildOrder
{
    int planet;
    int shipCount;

    BuildOrder(int p = 0, int s = 0) : planet(p), shipCount(s) {}
    BuildOrder(const BuildOrder & o) : planet(o.planet), shipCount(o.shipCount) {}
    ~BuildOrder() {}

    bool operator==(const BuildOrder & o) const
    {
        return  planet == o.planet &&
                shipCount == o.shipCount;
    }
};

Q_DECLARE_METATYPE(BuildOrder)

struct ShipMove
{
    int srcPlanet;
    int destPlanet;
    int shipCount;

    ShipMove(int src = 0, int dest = 0, int ship = 0) :
        srcPlanet(src), destPlanet(dest), shipCount(ship) {}
    ShipMove(const ShipMove & o) : srcPlanet(o.srcPlanet),
        destPlanet(o.destPlanet), shipCount(o.shipCount) {}
    ~ShipMove() {}

    bool operator==(const ShipMove & o) const
    {
        return  srcPlanet == o.srcPlanet &&
                destPlanet == o.destPlanet &&
                shipCount == o.shipCount;
    }
};

Q_DECLARE_METATYPE(ShipMove)

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
	int doubleSpeedRemainingTurns;
    int shipCount;
};

struct ScanResult
{
    int planet;
    int player;
    int resourcePerRound;
    int maxBuildPerRound;
    int shipCount;
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
    int remainingTurns;
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

// Display

struct InitDisplayPlanet
{
    int posX;
    int posY;
    int playerID;
    int shipCount;
    int planetSize;
};

struct TurnDisplayPlanet
{
    int playerID;
    int shipCount;
};


#endif // STRUCT_HPP
