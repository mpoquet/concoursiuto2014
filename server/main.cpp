#include <iostream>

#include <QApplication>
#include <QStringList>

#include <QTime>

#include "network.hpp"
#include "game.hpp"
#include "basicgamemodel.hpp"
#include "mainwidget.h"

using namespace std;

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);

	qsrand(QTime::currentTime().msec());

	Game * m = new Game("../map/example.map", 500, 100, new BasicGameModel(500));

	quint16 port = 4242;
	bool ok;

	if (argc > 1)
	{
		int p = app.arguments().at(1).toInt(&ok);

		if (!ok || p <= 1024 || p > 65535)
		{
			cout << "Invalid port. Must be in ]1024;65535]" << endl;
			return 0;
		}

		port = p;
	}

	Network * n = new Network(port, 4, 1);

	MainWidget * w = new MainWidget();

	QObject::connect(n, SIGNAL(loginPlayer(QTcpSocket*,QString)), m, SLOT(playerLogin(QTcpSocket*,QString)));
	QObject::connect(n, SIGNAL(movePlayer(QTcpSocket*,QVector<int>,QVector<BuildOrder>,QVector<ShipMove>)), m, SLOT(playerOrder(QTcpSocket*,QVector<int>,QVector<BuildOrder>,QVector<ShipMove>)));
	QObject::connect(m, SIGNAL(finishedSignal(QTcpSocket*,bool)), n, SLOT(sendFinished(QTcpSocket*,bool)));
	QObject::connect(m, SIGNAL(initPlayerSignal(QTcpSocket*,int,QVector<QVector<int> >,int,int,int)), n, SLOT(sendInitPlayer(QTcpSocket*,int,QVector<QVector<int> >,int,int,int)));
	QObject::connect(m, SIGNAL(turnSignal(QTcpSocket*,int,QVector<OurShipsOnPlanets>,QVector<ScanResult>,QVector<OurMovingShips>,QVector<IncomingEnnemyShips>,QVector<FightReport>)), n, SLOT(sendTurnPlayer(QTcpSocket*,int,QVector<OurShipsOnPlanets>,QVector<ScanResult>,QVector<OurMovingShips>,QVector<IncomingEnnemyShips>,QVector<FightReport>)));
	QObject::connect(w, SIGNAL(start()), m, SLOT(start()));

	n->run();
	w->show();

	return app.exec();
}
