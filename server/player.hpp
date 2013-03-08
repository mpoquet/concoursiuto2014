#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "struct.hpp"
#include "planet.hpp"

#include <QVector>

class Player
{
	public:
		Player(int id);
		~Player();


	protected:
		int m_id;
		int m_ressources;

		QVector<Planet*> m_planet;

		QVector<ShipMove> m_waitingMoves;
		QVector<BuildOrder> m_waitingBuild;
};

#endif // PLAYER_HPP
