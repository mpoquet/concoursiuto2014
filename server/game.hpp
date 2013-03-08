#ifndef GAME_HPP
#define GAME_HPP

#include "struct.hpp"
#include "planet.hpp"
#include "player.hpp"
#include "galaxy.hpp"
#include "abstractgamemodel.hpp"
#include "network.hpp"

#include <QObject>
#include <QVector>
#include <QTcpSocket>
#include <QString>

class Game : public QObject
{
	Q_OBJECT
	public:
		Game(QString mapFilename, int delayBetweenRound, AbstractGameModel * gameModel, Network * network);

		void start();
		void iteration();

	signals:
		void initPlayerSignal(QTcpSocket * socket,
							int planetCount,
							QVector<QVector<int> > distanceMatrix,
							int roundCount,
							int scanLimit);



	public slots:
		void playerLogin(QTcpSocket * socket, QString nickname);
		void displayLogin(QTcpSocket * socket, QString nickname);
		void playerOrder(QTcpSocket * socket,
						 QVector<int> planetsToScan,
						 QVector<BuildOrder> shipsToBuild,
						 QVector<ShipMove> shipsToMove);

	protected:
		Planet * getPlanet(int id);
		Galaxy * getGalaxy(int id);

		void filterBuildOrder(QVector<BuildOrder> & orders, Player * player);
		void filterShipMove(QVector<ShipMove> & shipMoves, Player * player);
		void filterScan(QVector<int> & scans);

		QVector<QVector<int> > getDistanceMatrix();

	private:
		int m_delayBetweenRound;

		AbstractGameModel * m_gameModel;
		Network * m_network;
		QVector<Galaxy*> m_galaxies;
		QVector<Player*> m_players;
		QVector<ShipMovement*> m_movements;
		QVector<Planet*> m_planets;
		QMap<Player*, QTcpSocket *> m_clientSockets;
};

#endif // GAME_HPP
