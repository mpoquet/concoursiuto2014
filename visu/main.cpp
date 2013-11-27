#include <QApplication>

#include "network.hpp"
#include "netwidget.hpp"
#include "graphicsviewer.hpp"
#include "sfmlviewer.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*if (argc == 1)
    {
        Network n;
        NetWidget w;
        w.show();

        SFMLViewer viewer;
        viewer.show();

        QObject::connect(&w, SIGNAL(wantToConnectToHost(QString,quint16)), &n, SLOT(connectToHost(QString,quint16)));
        QObject::connect(&w, SIGNAL(wantToLogin()), &n, SLOT(login()));

        QObject::connect(&n, SIGNAL(connected()), &w, SLOT(onConnected()));
        QObject::connect(&n, SIGNAL(disconnected()), &w, SLOT(onDisconnected()));
        QObject::connect(&n, SIGNAL(error()), &w, SLOT(onError()));
        QObject::connect(&n, SIGNAL(logged()), &w, SLOT(onLogged()));
        QObject::connect(&n, SIGNAL(cannotLogIn()), &w, SLOT(onCannotLogIn()));

        QObject::connect(&n, SIGNAL(initReceived(int,QVector<QVector<int> >,QVector<InitDisplayPlanet>,QVector<QString>,int)), &viewer, SLOT(onInit(int,QVector<QVector<int> >,QVector<InitDisplayPlanet>,QVector<QString>,int)));
        QObject::connect(&n, SIGNAL(turnReceived(QVector<int>, QVector<TurnDisplayPlanet>, QVector<ShipMovement>)), &viewer, SLOT(onTurn(QVector<int>, QVector<TurnDisplayPlanet>, QVector<ShipMovement>)));

        return a.exec();
    }
    else*/ if (argc == 3)
    {
        QString ip;
        quint16 port;
        bool ok;

        ip = a.arguments()[1];
        port = a.arguments()[2].toInt(&ok);

        if (!ok)
        {
            printf("Usage : visu\n"
                   "        visu ip port\n");
            return a.exec();
        }

        Network n;

        SFMLViewer viewer;
        viewer.show();

        QObject::connect(&n, SIGNAL(connected()), &n, SLOT(login()));
        QObject::connect(&n, SIGNAL(initReceived(int,QVector<QVector<int> >,QVector<InitDisplayPlanet>,QVector<QString>,int)), &viewer, SLOT(onInit(int,QVector<QVector<int> >,QVector<InitDisplayPlanet>,QVector<QString>,int)));
        QObject::connect(&n, SIGNAL(turnReceived(QVector<int>, QVector<TurnDisplayPlanet>, QVector<ShipMovement>)), &viewer, SLOT(onTurn(QVector<int>, QVector<TurnDisplayPlanet>, QVector<ShipMovement>)));
        QObject::connect(&viewer, SIGNAL(destroyed()), qApp, SLOT(quit()));

        n.connectToHost(ip, port);

        return a.exec();
    }
    else
    {
        printf("Usage : visu ip port\n");

        a.exit(0);
    }
}
