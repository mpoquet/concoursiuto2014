#ifndef SFMLVIEWER_HPP
#define SFMLVIEWER_HPP

#include <QTimer>
#include <QTime>
#include <QMap>
#include <QMutex>

#include <SFML/Graphics.hpp>
#include <QResizeEvent>

#include "abstractviewer.hpp"

class SFMLViewer : public AbstractViewer, public sf::RenderWindow
{
    Q_OBJECT

    struct Player
    {
        QString nick;
        sf::Color color;
        int score;

        Player(const QString & n = "???", sf::Color c = sf::Color::White, int s = 0) :
            nick(n), color(c), score(s) {}
    };

    struct Planet
    {
        sf::Sprite sprite;
        
        int size;
        int playerID;
        int shipCount;
        
        int posX, posY;
    };

public:
    explicit SFMLViewer(QWidget * parent = 0);

public slots:
    void onInit(int planetCount,
                QVector<QVector<int> > distanceMatrix,
                QVector<InitDisplayPlanet> planets,
                QVector<QString> playerNicks,
                int roundCount);

    void onTurn(QVector<int> scores,
                QVector<TurnDisplayPlanet> planets,
                QVector<ShipMovement> movements);

protected:
    void resizeEvent(QResizeEvent * e);

private :
	void scaleGame();
	
    void onDisplayInit();
    void onDisplayUpdate();

    QPaintEngine* paintEngine() const;
    void showEvent(QShowEvent * e);
    void paintEvent(QPaintEvent * e);
	
    QTimer _timer;
    QTime _time;
    bool _initialized, _started;

    int _planetCount;
    QVector<QVector<int> > _distance;

    QMap<int, Player> _players;
    QVector<Planet> _planets;
    QVector<sf::Sprite> _ships;

    int _roundCount;
    int _currentRound;

    sf::Texture _texturePlanet;
    QMutex _mutex;
    
    sf::Font _font;
    float _scale;
    
    int _minX, _maxX, _minY, _maxY;
};

#endif // SFMLVIEWER_HPP
