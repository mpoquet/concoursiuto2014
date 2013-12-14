#ifndef SFMLVIEWER_HPP
#define SFMLVIEWER_HPP

#include <QTimer>
#include <QTime>
#include <QMap>

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
        int shipCount;
        int planetCount;

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
    void onInit(QVector<InitDisplayPlanet> planets,
                QVector<QVector<int> > distanceMatrix,
                QVector<QString> playerNicks,
                int roundCount);

    void onTurn(QVector<int> scores,
                QVector<TurnDisplayPlanet> planets,
                QVector<ShipMovement> movements);

protected:
    void resizeEvent(QResizeEvent * e);
    void keyPressEvent(QKeyEvent * e);

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

    QMap<int, Player> _players;
    QVector<Planet> _planets;
    QVector<ShipMovement> _moves;
    QVector<QVector<int> > _distanceMatrix;

    int _roundCount;
    int _currentRound;

    sf::Texture _texturePlanet;
    sf::Texture _textureShip;

    sf::Font _font;
    float _scale;
    
    int _minX, _maxX, _minY, _maxY;

    bool _drawShips = false;
};

#endif // SFMLVIEWER_HPP
