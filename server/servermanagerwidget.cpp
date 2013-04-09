#include "servermanagerwidget.hpp"

#include <QMessageBox>

ServerManagerWidget::ServerManagerWidget(Game * gameEngine, Network * network, QWidget * parent) :
	QWidget(parent)
{
	m_gameEngine = gameEngine;
	m_network = network;

	m_clientId = 1;
	m_displayId = 1;

	m_log = new QTextEdit();
	m_log->setReadOnly(true);

	connect(m_network, SIGNAL(clientConnected(QTcpSocket*)), this, SLOT(clientConnected(QTcpSocket*)));
	connect(m_network, SIGNAL(loginPlayer(QTcpSocket*,QString)), this, SLOT(playerConnected(QTcpSocket*,QString)));
	connect(m_network, SIGNAL(loginDisplay(QTcpSocket*)), this, SLOT(displayConnected(QTcpSocket*)));
	connect(m_network, SIGNAL(displayDisconnected(QTcpSocket*)), this, SLOT(displayDisconnected(QTcpSocket*)));
	connect(m_network, SIGNAL(playerDisconnected(QTcpSocket*)), this, SLOT(playerDisconnected(QTcpSocket*)));
	connect(m_network, SIGNAL(unloggedClientDisconnected(QTcpSocket*)), this, SLOT(clientDisconnected(QTcpSocket*)));

	m_clientsList = new QListWidget();
	m_displaysList = new QListWidget();
	m_playersList = new QListWidget();

	QVBoxLayout * clientsListLayout = new QVBoxLayout();
	QVBoxLayout * displaysListLayout = new QVBoxLayout();
	QVBoxLayout * playersListLayout = new QVBoxLayout();

	clientsListLayout->addWidget(new QLabel("clients"));
	clientsListLayout->addWidget(m_clientsList);
	displaysListLayout->addWidget(new QLabel("displays"));
	displaysListLayout->addWidget(m_displaysList);
	playersListLayout->addWidget(new QLabel("players"));
	playersListLayout->addWidget(m_playersList);

	QHBoxLayout * connectionStateLayout = new QHBoxLayout();

	connectionStateLayout->addLayout(clientsListLayout);
	connectionStateLayout->addLayout(playersListLayout);
	connectionStateLayout->addLayout(displaysListLayout);


	m_roundCount = new QSpinBox();
	m_delayBetweenRound = new QSpinBox();

	m_roundCount->setRange(0, 100000);
	m_delayBetweenRound->setRange(0, 100000);

	m_roundCount->setValue(250);
	m_delayBetweenRound->setValue(500);

    m_startButton = new QPushButton("Start");

	QHBoxLayout * layoutGameParameters = new QHBoxLayout();
	layoutGameParameters->addWidget(new QLabel("Nombre de tours de jeu : "));
	layoutGameParameters->addWidget(m_roundCount);
	layoutGameParameters->addWidget(new QLabel("Temps entre les tours (ms) : "));
	layoutGameParameters->addWidget(m_delayBetweenRound);
    layoutGameParameters->addWidget(m_startButton);

    connect(m_startButton, SIGNAL(clicked()), this, SLOT(startClicked()));

	QVBoxLayout * windowsLayout = new QVBoxLayout();

	windowsLayout->addLayout(connectionStateLayout);
	windowsLayout->addWidget(m_log);
	windowsLayout->addLayout(layoutGameParameters);

	this->setLayout(windowsLayout);
	this->setWindowTitle("Server Manager");
}

void ServerManagerWidget::clientConnected(QTcpSocket* s)
{
    m_clients.insert(s, QString("Anonymous_%1").arg(QString::number(m_clientId)));
	addMessage(QString("<strong>Client connection</strong> : Anonymous_%1").arg(QString::number(m_clientId)));
	m_clientId++;
	refreshClient();
}

void ServerManagerWidget::playerConnected(QTcpSocket* s, QString login)
{
	m_clients.remove(s);
	m_players.insert(s, login);
	addMessage(QString("<strong>Player connection</strong> : %1").arg(login));
	refreshPlayer();
	refreshClient();
}

void ServerManagerWidget::displayConnected(QTcpSocket* s)
{
	m_clients.remove(s);
	m_displays.insert(s, QString("Display_%1").arg(m_displayId));
	addMessage(QString("<strong>Display connection</strong> : Display_%1").arg(m_displayId));
	m_displayId++;
	refreshDisplay();
	refreshClient();
}

void ServerManagerWidget::clientDisconnected(QTcpSocket * s)
{
	addMessage(QString("<strong>Client deconnection</strong> : %1").arg(m_clients[s]));
	m_clients.remove(s);
	refreshClient();
}

void ServerManagerWidget::displayDisconnected(QTcpSocket* s)
{
	addMessage(QString("<strong>Display deconnection</strong> : %1").arg(m_clients[s]));
	m_displays.remove(s);
	refreshDisplay();
}

void ServerManagerWidget::playerDisconnected(QTcpSocket* s)
{
	addMessage(QString("<strong>Player deconnection</strong> : %1").arg(m_clients[s]));
	m_players.remove(s);
	refreshPlayer();
}

void ServerManagerWidget::startClicked()
{
    if (m_players.size() < 2)
        QMessageBox::information(this, "Information", "You can't start a game without at least 2 players");
    else
    {
        m_gameEngine->setDelayBetweenRound(m_delayBetweenRound->value());
        m_gameEngine->setNumberOfRound(m_roundCount->value());
        m_gameEngine->start();

        m_startButton->setEnabled(false);
    }
}

void ServerManagerWidget::refreshClient()
{
	m_clientsList->clear();

    QList<QTcpSocket*> keys = m_clients.keys();

    for (int i = 0; i < keys.size(); ++i)
        if (m_clients[keys[i]].isEmpty())
            m_clients.remove(keys[i]);

	m_clientsList->addItems(m_clients.values());
}

void ServerManagerWidget::refreshPlayer()
{
	m_playersList->clear();
	m_playersList->addItems(m_players.values());
}

void ServerManagerWidget::refreshDisplay()
{
	m_displaysList->clear();
	m_displaysList->addItems(m_displays.values());
}

void ServerManagerWidget::addMessage(QString msg)
{
	m_log->setHtml(m_log->toHtml() + msg);
}
