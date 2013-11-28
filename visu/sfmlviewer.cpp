#include "sfmlviewer.hpp"

#include <SFML/OpenGL.hpp>

#include <QDebug>

#define LEGEND_WIDTH 160

SFMLViewer::SFMLViewer(QWidget *parent) :
    AbstractViewer(parent),
    _initialized(false), _started(false)
{
    // Mise en place de quelques options pour autoriser le rendu direct dans le widget
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // Changement de la police de focus, pour autoriser notre widget à capter les évènements clavier
    setFocusPolicy(Qt::StrongFocus);

    QWidget::resize(640 + LEGEND_WIDTH,480);

    // Préparation du timer
    _timer.setInterval(50);
}

void SFMLViewer::scaleGame()
{
	if(_started)
	{
		_scale = std::max(std::min(size().width(), size().height()) / _planets.size(), 1);

		float aW = (float) (QWidget::width() - _scale * 2 - LEGEND_WIDTH) / (_maxX - _minX);
		float aH = (float) (QWidget::height() - _scale * 2) / (_maxY - _minY);
		
		for (int i = 0; i < _planets.size(); ++i)		
		{		
			_planets[i].sprite.setPosition(LEGEND_WIDTH + _scale + aW * (_planets[i].posX - _minX),
										   _scale + aH * (_planets[i].posY - _minY));
		   
			_planets[i].sprite.setScale(_scale / _planets[i].sprite.getTexture()->getSize().x,
										_scale / _planets[i].sprite.getTexture()->getSize().y);
		}
	}
}

void SFMLViewer::onInit(int planetCount,
                        QVector<QVector<int> > distanceMatrix,
                        QVector<InitDisplayPlanet> planets,
                        QVector<QString> playerNicks,
                        int roundCount)
{
	qDebug() << "SFML : onInit()";
	
	_started =  true;
	
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

    _minX = _minY = 2147483647; // Mouhahahaha
    _maxX = _maxY = -2147483647;

    for (int i = 0; i < planets.size(); ++i)
    {
        if (planets[i].posX < _minX)
            _minX = planets[i].posX;

        if (planets[i].posX > _maxX)
            _maxX = planets[i].posX;

        if (planets[i].posY < _minY)
            _minY = planets[i].posY;

        if (planets[i].posY > _maxY)
            _maxY = planets[i].posY;
    }
	
    _mutex.lock();

    _planets.resize(_planetCount);
    for (int i = 0; i < _planets.size(); ++i)		
    {		
        _planets[i].playerID = planets[i].playerID;
        _planets[i].shipCount = planets[i].shipCount;
        _planets[i].size = planets[i].planetSize;

        _planets[i].sprite.setTexture(_texturePlanet);
        _planets[i].sprite.setOrigin(_texturePlanet.getSize().x/2, _texturePlanet.getSize().y/2);
        _planets[i].sprite.setColor(_players[_planets[i].playerID].color);
        
        _planets[i].posX = planets[i].posX;
        _planets[i].posY = planets[i].posY;
    }
    
    scaleGame();

    _mutex.unlock();

    qDebug() << "SFML : init received";
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

void SFMLViewer::onDisplayInit()
{
    _time.start();
	
	_font.loadFromFile("arial.ttf");
	
    _texturePlanet.loadFromFile("img/planet.png");
    _texturePlanet.setSmooth(true);
}

void SFMLViewer::onDisplayUpdate()
{
	glViewport(0, 0, size().width(), size().height());
	
    clear();

	if(!_started)
	{
		sf::Text waiting("Waiting ...", _font);
		waiting.setCharacterSize(45);
		waiting.setPosition(size().width() / 2 - waiting.getLocalBounds().width / 2, 
						 size().height() / 2 - waiting.getLocalBounds().height / 2);
		waiting.setColor(sf::Color(255,255,255));
		
		draw(waiting);
	}
	else
	{
		_mutex.lock();
		
		// Legend
		sf::RectangleShape rectangle;
		rectangle.setSize(sf::Vector2f(LEGEND_WIDTH, size().height()));
		rectangle.setFillColor(sf::Color(180,180,200));
		rectangle.setPosition(0, 0);
		
		draw(rectangle);
		
		int count = 0;
		for(QMap<int, Player>::iterator i = _players.begin(); i != _players.end(); ++i, ++count)
		{
			Player &p = i.value();
			float localScale = 64;
			
			sf::Sprite sprite;
			sprite.setTexture(_texturePlanet);
			sprite.setPosition(4, count * localScale);
			sprite.setScale(localScale / _texturePlanet.getSize().x, localScale / _texturePlanet.getSize().y);
			sprite.setColor(p.color);
			
			draw(sprite);
			
			sf::Text text(p.nick.toStdString(), _font);
			text.setCharacterSize(16);
			text.setPosition(8 + localScale, count * localScale + localScale / 4);
			text.setColor(sf::Color(0, 0, 0));
			
			draw(text);
		}
		
		QString roundString = QString::number(_currentRound) + "/" + QString::number(_roundCount);
		
		sf::Text rounds(roundString.toStdString(), _font);
		rounds.setCharacterSize(20);
		rounds.setPosition(8, size().height() - rounds.getLocalBounds().height * 2);
		rounds.setColor(sf::Color(0, 0, 0));
		
		draw(rounds);
			
		for(QVector<Planet>::iterator i = _planets.begin(); i != _planets.end(); ++i)
		{		
			Planet &planet = *i;
			Player &player = _players[planet.playerID];
			
			draw(planet.sprite);
			
			if(player.nick.toStdString() != "Empty")
			{
				sf::Text nick(player.nick.toStdString(), _font);
				nick.setCharacterSize(16);			
				nick.setOrigin(nick.getLocalBounds().width / 2, nick.getLocalBounds().height / 2);
				nick.setPosition(planet.sprite.getPosition().x, planet.sprite.getPosition().y + _scale / 2);
				nick.setColor(player.color);
				
				draw(nick);
			}
			
			sf::Text ships(QString::number(planet.shipCount).toStdString(), _font);
			ships.setCharacterSize(14);		
			ships.setOrigin(ships.getLocalBounds().width / 2, ships.getLocalBounds().height / 2);
			ships.setPosition(planet.sprite.getPosition().x, planet.sprite.getPosition().y);
			ships.setColor(sf::Color(0, 0, 0));
			
			draw(ships);
		}
		
		_mutex.unlock();
	}
}

void SFMLViewer::resizeEvent(QResizeEvent *e)
{
	sf::RenderWindow::setSize(sf::Vector2u(e->size().width(), e->size().height()));
    
	sf::View view(sf::FloatRect(0, 0, e->size().width(), e->size().height()));
	view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
	
    sf::RenderWindow::setView(view);
	
    onResize();
    
    scaleGame();
    
    e->accept();
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
