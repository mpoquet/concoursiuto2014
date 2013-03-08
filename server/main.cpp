#include <QGuiApplication>

#include <QTime>

#include "network.hpp"
#include "game.hpp"

int main(int argc, char ** argv)
{
	QGuiApplication(argc, argv);
	qsrand(QTime::currentTime().msec());


	Game m("../map/example.map", 500, nullptr, nullptr);

	Network n;

	return 0;
}
