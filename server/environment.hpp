#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "struct.hpp"
#include "planet.hpp"

#include <QVector>
#include <QObject>

class Environment : public QObject
{
	Q_OBJECT
	public:
		Environment();
		~Environment();

	private:
		QVector<Player*> m_players;
		QVector<ShipMovement*> m_movements;
		QVector<Planet*> m_planets;
};

#endif // ENVIRONMENT_HPP
