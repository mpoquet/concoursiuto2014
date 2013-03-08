#ifndef GAME_HPP
#define GAME_HPP

#include "environment.hpp"

#include <QObject>
#include <QVector>
#include <QTcpSocket>

class Game : public QObject
{
	public:
		Game();

	private:
		Environment * m_environment;
		QMap<int, QTcpSocket *> m_clientSockets;
};

#endif // GAME_HPP
