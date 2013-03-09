#include <iostream>

#include <QApplication>
#include <QStringList>

#include "widget.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList args = a.arguments();
    bool invalidParameters = false;

    if (args.size() != 3)
        invalidParameters = true;

    if (invalidParameters)
    {
        cout << "Usage: qtnc address port" << endl;
        return 0;
    }

    Widget w;
    w.show();
    w.run(args[1], args[2].toInt());
    
    return a.exec();
}
