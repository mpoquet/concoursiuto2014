#include <QApplication>

#include "network.hpp"
#include "netwidget.hpp"
#include "graphicsviewer.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Network n;
    NetWidget w;
    w.show();

    GraphicsViewer viewer;
    viewer.show();

    QObject::connect(&w, SIGNAL(wantToConnectToHost(QString,quint16)), &n, SLOT(connectToHost(QString,quint16)));
    QObject::connect(&w, SIGNAL(wantToLogin()), &n, SLOT(login()));

    QObject::connect(&n, SIGNAL(connected()), &w, SLOT(onConnected()));
    QObject::connect(&n, SIGNAL(disconnected()), &w, SLOT(onDisconnected()));
    QObject::connect(&n, SIGNAL(error()), &w, SLOT(onError()));
    QObject::connect(&n, SIGNAL(logged()), &w, SLOT(onLogged()));
    QObject::connect(&n, SIGNAL(cannotLogIn()), &w, SLOT(onCannotLogIn()));

    QObject::connect(&n, SIGNAL(initReceived(int,QVector<QVector<int> >,QVector<InitDisplayPlanet>,QVector<QString>,int)), &viewer, SLOT(onInit(int,QVector<QVector<int> >,QVector<InitDisplayPlanet>,QVector<QString>,int)));
    
    return a.exec();
}
