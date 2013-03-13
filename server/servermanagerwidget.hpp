#ifndef SERVERMANAGERWIDGET_HPP
#define SERVERMANAGERWIDGET_HPP

#include <QWidget>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QTcpSocket>

#include <QMap>
#include <QVector>
#include <QSpinBox>
#include <QPushButton>

#include "game.hpp"
#include "network.hpp"

class ServerManagerWidget : public QWidget
{
	Q_OBJECT
	public:
		ServerManagerWidget(Game * gameEngine, Network * network, QWidget *parent = 0);

	signals:

	public slots:
		void clientConnected(QTcpSocket* s);
		void playerConnected(QTcpSocket* s, QString login);
		void displayConnected(QTcpSocket* s);

		void clientDisconnected(QTcpSocket * s);
		void displayDisconnected(QTcpSocket* s);
		void playerDisconnected(QTcpSocket* s);

		void startClicked();

	protected:
		void refreshClient();
		void refreshPlayer();
		void refreshDisplay();
		void addMessage(QString msg);

	protected:
		Game * m_gameEngine;
		Network * m_network;

		int m_clientId;
		int m_displayId;

		QMap<QTcpSocket *, QString> m_clients;
		QMap<QTcpSocket *, QString> m_players;
		QMap<QTcpSocket *, QString> m_displays;

		QListWidget * m_clientsList;
		QListWidget * m_playersList;
		QListWidget * m_displaysList;

		QTextEdit * m_log;

		QSpinBox * m_roundCount;
		QSpinBox * m_delayBetweenRound;
};

#endif // SERVERMANAGERWIDGET_HPP
