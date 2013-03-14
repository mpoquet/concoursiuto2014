#include "sfmlviewer.hpp"

#include <QDebug>

SFMLViewer::SFMLViewer(QWidget *parent) :
    AbstractViewer(parent),
    _initialized(false)
{
    // Mise en place de quelques options pour autoriser le rendu direct dans le widget
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // Changement de la police de focus, pour autoriser notre widget à capter les évènements clavier
    setFocusPolicy(Qt::StrongFocus);

    QWidget::resize(640,480);

    // Préparation du timer
    _timer.setInterval(50);
}

void SFMLViewer::test()
{
    int planetCount;
    QVector<QVector<int> > distanceMatrix;
    QVector<InitDisplayPlanet> planets;
    QVector<QString> playerNicks;
    int roundCount;

    planetCount = 4;

    distanceMatrix.resize(planetCount);
    for (int i = 0; i < distanceMatrix.size(); ++i)
        distanceMatrix.resize(planetCount);

    planets.resize(planetCount);

    planets[0].posX = 0;
    planets[0].posY = 0;
    planets[0].playerID = -1;

    planets[1].posX = 50;
    planets[1].posY = 0;
    planets[1].playerID = 0;

    planets[2].posX = 100;
    planets[2].posY = 100;
    planets[2].playerID = 1;

    planets[3].posX = 75;
    planets[3].posY = 75;
    planets[3].playerID = 2;

    playerNicks.resize(2);
    playerNicks[0] = "Bouh";
    playerNicks[1] = "Ada";
    roundCount = 42;

    onInit(planetCount, distanceMatrix, planets, playerNicks, roundCount);
}

void SFMLViewer::onTurn(QVector<int> scores,
                        QVector<TurnDisplayPlanet> planets,
                        QVector<ShipMovement> movements)
{
    qDebug() << "SFML : turn received";
}

void SFMLViewer::resizeEvent(QResizeEvent *e)
{
    onResize();
    e->accept();
}

void SFMLViewer::onInit(int planetCount,
                        QVector<QVector<int> > distanceMatrix,
                        QVector<InitDisplayPlanet> planets,
                        QVector<QString> playerNicks,
                        int roundCount)
{
    _roundCount = roundCount;
    _currentRound = 0;

    // Let's handle players
    _players[-1] = Player("Empty");
    _players[0] = Player("Autochtone", sf::Color(127,127,127));

    for (int i = 0; i < playerNicks.size(); ++i)
        _players[i+1] = Player(playerNicks[i]);

    // Player colors
    QVector<QColor> colors(playerNicks.size());
    int h = 360 / colors.size();

    for (int i = 0; i < colors.size(); ++i)
        colors[i] = QColor::fromHsl(h*i, 100, 100);

    for (int i = 0; i < playerNicks.size(); ++i)
        _players[i+1].color = sf::Color(colors[i].red(), colors[i].green(), colors[i].blue());


    // Planets
    _planetCount = planetCount;
    _distance = distanceMatrix;

    int minX,minY,maxX,maxY;
    minX = minY = 2147483647; // Mouhahahaha
    maxX = maxY = -2147483647;

    for (int i = 0; i < planets.size(); ++i)
    {
        if (planets[i].posX < minX)
            minX = planets[i].posX;

        if (planets[i].posX > maxX)
            maxX = planets[i].posX;

        if (planets[i].posY < minY)
            minY = planets[i].posY;

        if (planets[i].posY > maxY)
            maxY = planets[i].posY;
    }

    float radiusPX = QWidget::width() / planetCount / 2;
    float scale = _texturePlanet.getSize().x / radiusPX;

    qDebug() << "radius, scale :" << radiusPX << scale;

    qDebug() << "min" << minX << minY;
    qDebug() << "max" << maxX << maxY;

    float aW, aH;
    aW = (float) QWidget::width() / (maxX - minX);
    aH = (float) QWidget::height() / (maxY - minY);

    _mutex.lock();

    _planets.resize(_planetCount);
    for (int i = 0; i < _planets.size(); ++i)
    {
        _planets[i].playerID = planets[i].playerID;
        _planets[i].shipCount = planets[i].shipCount;
        _planets[i].size = planets[i].planetSize;

        _planets[i].sprite.setTexture(_texturePlanet);
        _planets[i].sprite.setOrigin(_texturePlanet.getSize().x/2, _texturePlanet.getSize().y/2);
        _planets[i].sprite.setPosition(aW * (planets[i].posX - minX), aH * (planets[i].posY - minY));
        _planets[i].sprite.setScale(0.5f, 0.5f);
        _planets[i].sprite.setColor(_players[_planets[i].playerID].color);
    }

    _mutex.unlock();

    qDebug() << "SFML : init received";
}

void SFMLViewer::onDisplayInit()
{
    _time.start();

    _texturePlanet.loadFromFile("img/planet.png");
    _texturePlanet.setSmooth(true);

    _sprite.setTexture(_texturePlanet);
    _sprite.setOrigin(_texturePlanet.getSize().x / 2, _texturePlanet.getSize().y / 2);
    _sprite.move(size().width() / 2, size().height() / 2);
}

void SFMLViewer::onDisplayUpdate()
{
    clear();

    _mutex.lock();
    for (int i = 0; i < _planets.size(); ++i)
        draw(_planets[i].sprite);
    _mutex.unlock();

    static bool first = true;
    if (first)
    {
        test();
        first = false;
    }
}

QPaintEngine *SFMLViewer::paintEngine() const
{
    return 0;
}

#ifdef Q_WS_X11
#include <Qt/qx11info_x11.h>
#include <X11/Xlib.h>
#endif

void SFMLViewer::showEvent(QShowEvent * e)
{
    if (!_initialized)
    {
        // Sous X11, il faut valider les commandes qui ont été envoyées au serveur
        // afin de s'assurer que SFML aura une vision à jour de la fenêtre
#ifdef Q_WS_X11
        XFlush(QX11Info::display());
#endif

        sf::ContextSettings settings;
        settings.antialiasingLevel = 16;

        // On crée la fenêtre SFML avec l'identificateur du widget
        sf::RenderWindow::create(winId(), settings);

        // On laisse la classe dérivée s'initialiser si besoin
        onDisplayInit();

        // On paramètre le timer de sorte qu'il génère un rafraîchissement à la fréquence souhaitée
        connect(&_timer, SIGNAL(timeout()), this, SLOT(repaint()));
        _timer.start();

        _initialized = true;
    }
    e->accept();
}

void SFMLViewer::paintEvent(QPaintEvent *e)
{
    onDisplayUpdate();
    display();
    e->accept();
}
