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

    QSizePolicy policy;
    policy.setHeightForWidth(true);
    setSizePolicy(policy);

    // Changement de la police de focus, pour autoriser notre widget à capter les évènements clavier
    setFocusPolicy(Qt::StrongFocus);

    //QWidget::resize(1024, 768);

    // Préparation du timer
    _timer.setInterval(50);
}

void SFMLViewer::onTurn(QVector<int> scores,
                        QVector<TurnDisplayPlanet> planets,
                        QVector<ShipMovement> movements)
{
    qDebug() << "SFML : turn received";
}

int SFMLViewer::heightForWidth(int w) const
{
    return w;
}

void SFMLViewer::resizeEvent(QResizeEvent *e)
{
//    QSize size = e->size();
//    size.scale(size.width(), size.width() * 0.75f, Qt::IgnoreAspectRatio);

//    int x = pos().x();
//    int y = pos().y();
//    setGeometry(x, y, size.width(), size.height());

    onResize();
}

void SFMLViewer::onInit(int planetCount,
                        QVector<QVector<int> > distanceMatrix,
                        QVector<InitDisplayPlanet> planets,
                        QVector<QString> playerNicks,
                        int roundCount)
{
    qDebug() << "SFML : init received";
}

void SFMLViewer::onDisplayInit()
{
    _time.start();
    _texture.loadFromFile("img/planet.png");
    _texture.setSmooth(true);

    _sprite.setTexture(_texture);
    _sprite.setOrigin(_texture.getSize().x / 2, _texture.getSize().y / 2);
    _sprite.move(size().width() / 2, size().height() / 2);
}

void SFMLViewer::onDisplayUpdate()
{
    clear();

    _sprite.rotate(1.0f);

    static int i = 127;
    static bool j = true;
    int k = 5;

    if (j)
        i += k;
    else
        i -= k;

    if (i > 255)
    {
        i = 255;
        j = false;
    }
    else if (i < 127)
    {
        i = 127;
        j = true;
    }

    sf::Color color(i, i, i);

    _sprite.setColor(color);
    _sprite.setScale(i / 127.0, i / 127.0);

    draw(_sprite);
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
}

void SFMLViewer::paintEvent(QPaintEvent *e)
{
    onDisplayUpdate();
    display();
}
