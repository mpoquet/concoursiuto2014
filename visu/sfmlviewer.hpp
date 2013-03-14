#ifndef SFMLVIEWER_HPP
#define SFMLVIEWER_HPP

#include <QTimer>
#include <QTime>

#include <SFML/Graphics.hpp>
#include <QResizeEvent>

#include "abstractviewer.hpp"

class SFMLViewer : public AbstractViewer, public sf::RenderWindow
{
    Q_OBJECT
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
    int heightForWidth(int w) const;
    void resizeEvent(QResizeEvent * e);

private :
    void onDisplayInit();
    void onDisplayUpdate();

    QPaintEngine* paintEngine() const;
    void showEvent(QShowEvent * e);
    void paintEvent(QPaintEvent * e);

    QTimer _timer;
    QTime _time;
    bool _initialized;
    sf::Texture _texture;
    sf::Sprite _sprite;
};

#endif // SFMLVIEWER_HPP
