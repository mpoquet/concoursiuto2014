#include <iostream>

#include <QGuiApplication>
#include <QStringList>

#include "network.hpp"

using namespace std;

int main(int argc, char ** argv)
{
    QGuiApplication app(argc, argv);

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

    Network n(port);
    n.run();

    return app.exec();
}
