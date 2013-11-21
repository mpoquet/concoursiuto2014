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

void SFMLViewer::onTurn(QVector<int> scores,
                        QVector<TurnDisplayPlanet> planets,
                        QVector<ShipMovement> movements)
{    
    _currentRound++;
    
    _mutex.lock();

    _planets.resize(_planetCount);
    for (int i = 0; i < _planets.size(); ++i)
    {
		_planets[i].playerID = planets[i].playerID;
        _planets[i].shipCount = planets[i].shipCount;
        _planets[i].sprite.setColor(_players[_planets[i].playerID].color);
    }

    _mutex.unlock();
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
	qDebug() << "SFML : onInit()";
	
    _roundCount = roundCount;
    _currentRound = 0;

    // Let's handle players
    _players[-1] = Player("Empty", sf::Color(255, 255, 255));
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

    _radiusPX = QWidget::width() / planetCount / 2;
    //float scale = _texturePlanet.getSize().x / _radiusPX;

    float aW = (float) (QWidget::width() - _radiusPX * 2) / (maxX - minX);
    float aH = (float) (QWidget::height() - _radiusPX * 2)/ (maxY - minY);

    _mutex.lock();

    _planets.resize(_planetCount);
    for (int i = 0; i < _planets.size(); ++i)		
    {		
        _planets[i].playerID = planets[i].playerID;
        _planets[i].shipCount = planets[i].shipCount;
        _planets[i].size = planets[i].planetSize;

        _planets[i].sprite.setTexture(_texturePlanet);
        _planets[i].sprite.setOrigin(_texturePlanet.getSize().x/2, _texturePlanet.getSize().y/2);
        _planets[i].sprite.setPosition(_radiusPX + aW * (planets[i].posX - minX),
                                       _radiusPX + aH * (planets[i].posY - minY));
        _planets[i].sprite.setScale(_radiusPX / _planets[i].sprite.getTexture()->getSize().x,
                                    _radiusPX / _planets[i].sprite.getTexture()->getSize().y);
        _planets[i].sprite.setColor(_players[_planets[i].playerID].color);
    }

    _mutex.unlock();
    
    _font.loadFromFile("arial.ttf");

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
    {
        draw(_planets[i].sprite);
        
        sf::Text text(_players[_planets[i].playerID].nick.toStdString(), _font);
		text.setCharacterSize(15);
		text.setPosition(_planets[i].sprite.getPosition().x - _radiusPX / 2, 
			_planets[i].sprite.getPosition().y + _radiusPX / 2);
		text.setColor(_players[_planets[i].playerID].color);
		
		draw(text);
    }
    _mutex.unlock();
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
