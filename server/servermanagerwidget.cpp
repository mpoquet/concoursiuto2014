#include "servermanagerwidget.hpp"

ServerManagerWidget::ServerManagerWidget(Game * gameEngine, Network * network, QWidget * parent) :
	QWidget(parent)
{
	m_gameEngine = gameEngine;
	m_network = network;

	m_clientId = 1;
	m_displayId = 1;

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

	QPushButton * startButton = new QPushButton("Start");

	QHBoxLayout * layoutGameParameters = new QHBoxLayout();
	layoutGameParameters->addWidget(new QLabel("Nombre de tours de jeu : "));
	layoutGameParameters->addWidget(m_roundCount);
	layoutGameParameters->addWidget(new QLabel("Temps entre les tours (ms) : "));
	layoutGameParameters->addWidget(m_delayBetweenRound);
	layoutGameParameters->addWidget(startButton);

	connect(startButton, SIGNAL(clicked()), this, SLOT(startClicked()));

	QVBoxLayout * windowsLayout = new QVBoxLayout();

	windowsLayout->addLayout(connectionStateLayout);
	windowsLayout->addLayout(layoutGameParameters);

	this->setLayout(windowsLayout);
	this->setWindowTitle("Server Manager");
}

void ServerManagerWidget::clientConnected(QTcpSocket* s)
{
	m_clients.insert(s, QString("Anonymous_%1").arg(QString::number(m_clientId)));
	m_clientId++;
	refreshClient();
}

void ServerManagerWidget::playerConnected(QTcpSocket* s, QString login)
{
	m_clients.remove(s);
	m_players.insert(s, login);
	refreshPlayer();
	refreshClient();
}

void ServerManagerWidget::displayConnected(QTcpSocket* s)
{
	m_clients.remove(s);
	m_displays.insert(s, QString("Display_%1").arg(m_displayId));
	m_displayId++;
	refreshDisplay();
	refreshClient();
}

void ServerManagerWidget::clientDisconnected(QTcpSocket * s)
{
	m_clients.remove(s);
	refreshClient();
}

void ServerManagerWidget::displayDisconnected(QTcpSocket* s)
{
	m_displays.remove(s);
	refreshDisplay();
}

void ServerManagerWidget::playerDisconnected(QTcpSocket* s)
{
	m_players.remove(s);
	refreshPlayer();
}

void ServerManagerWidget::startClicked()
{
	m_gameEngine->setDelayBetweenRound(m_delayBetweenRound->value());
	m_gameEngine->setNumberOfRound(m_roundCount->value());
	m_gameEngine->start();
}

void ServerManagerWidget::refreshClient()
{
	m_clientsList->clear();
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
