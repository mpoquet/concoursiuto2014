#include <QString>
#include <QtTest>
#include <QTcpSocket>
#include <QDebug>

#include "../server/network.hpp"
#include "../server/protocole.h"

class ServerReceive : public QObject
{
	Q_OBJECT
public:
	ServerReceive();

private Q_SLOTS:
	void initTestCase();
    void testConnection();
	void testLogin();
    void testFinished();

private:
	Network n;
    int delay;
};

ServerReceive::ServerReceive() :
    n(4242, 2, 1),
    delay(100)
{
}

void ServerReceive::initTestCase()
{
	n.run();
	QVERIFY(n.isListening());
}

void ServerReceive::testConnection()
{
	QCOMPARE(n.clientCount(), 0);

	QTcpSocket sock;
	QCOMPARE(sock.state(), QAbstractSocket::UnconnectedState);

    qRegisterMetaType<QTcpSocket*>("QTcpSocket*");
    QSignalSpy spyConnected(&n, SIGNAL(clientConnected(QTcpSocket*)));
    QSignalSpy spyDisconnected(&n, SIGNAL(unloggedClientDisconnected(QTcpSocket*)));

    sock.connectToHost("127.0.0.1", 4242);

    while (spyConnected.count() == 0)
        QTest::qWait(delay);

    QCOMPARE(spyConnected.count(), 1);
	QCOMPARE(sock.state(), QAbstractSocket::ConnectedState);
	QCOMPARE(n.clientCount(), 1);

    sock.disconnectFromHost();

    while (spyDisconnected.count() == 0)
        QTest::qWait(delay);

    QCOMPARE(spyDisconnected.count(), 1);
}

void ServerReceive::testLogin()
{
	QByteArray response, expected, send;
    QTcpSocket sockP, sockP2, sockP3, sockD, sockD2;

    QSignalSpy spyConnected(&n, SIGNAL(clientConnected(QTcpSocket*)));
    QSignalSpy spyPlayerDisconnected(&n, SIGNAL(playerDisconnected(QTcpSocket*)));
    QSignalSpy spyDisplayDisconnected(&n, SIGNAL(displayDisconnected(QTcpSocket*)));

    sockP.connectToHost("127.0.0.1", 4242);
    sockP2.connectToHost("127.0.0.1", 4242);
    sockP3.connectToHost("127.0.0.1", 4242);
    sockD.connectToHost("127.0.0.1", 4242);
    sockD2.connectToHost("127.0.0.1", 4242);

    while(spyConnected.count() != 5)
        QTest::qWait(delay);

    // Simple login player
    send = QString("%1bouh\n").arg(LOGIN_PLAYER).toLatin1();
    expected = QString("%1\n").arg(OK).toLatin1();
    sockP.write(send);

    QTest::qWait(delay);
    response = sockP.readAll();
    QCOMPARE(response, expected);

    // A player wants to login again as a player
    send = QString("%1bouh\n").arg(LOGIN_PLAYER).toLatin1();
    expected = QString("%1\n").arg(ALREADY_LOGGED).toLatin1();
    sockP.write(send);

    QTest::qWait(delay);
    response = sockP.readAll();
    QCOMPARE(response, expected);

    // A player wants to login as a display
    send = QString("%1bouh\n").arg(LOGIN_DISPLAY).toLatin1();
    expected = QString("%1\n").arg(ALREADY_LOGGED).toLatin1();
    sockP.write(send);

    QTest::qWait(delay);
    response = sockP.readAll();
    QCOMPARE(response, expected);

    // Simple login display
    send = QString("%1bouh\n").arg(LOGIN_DISPLAY).toLatin1();
    expected = QString("%1\n").arg(OK).toLatin1();
    sockD.write(send);

    QTest::qWait(delay);
    response = sockD.readAll();
    QCOMPARE(response, expected);

    // A display wants to login again as a display
    send = QString("%1bouh\n").arg(LOGIN_DISPLAY).toLatin1();
    expected = QString("%1\n").arg(ALREADY_LOGGED).toLatin1();
    sockD.write(send);

    QTest::qWait(delay);
    response = sockD.readAll();
    QCOMPARE(response, expected);

    // A display wants to login as a player
    send = QString("%1bouh\n").arg(LOGIN_PLAYER).toLatin1();
    expected = QString("%1\n").arg(ALREADY_LOGGED).toLatin1();
    sockD.write(send);

    QTest::qWait(delay);
    response = sockD.readAll();
    QCOMPARE(response, expected);

    // A second display wants to connect, but the max display count is set to 1
    send = QString("%1bouh\n").arg(LOGIN_DISPLAY).toLatin1();
    expected = QString("%1\n").arg(NO_MORE_ROOM).toLatin1();
    sockD2.write(send);

    QTest::qWait(delay);
    response = sockD2.readAll();
    QCOMPARE(response, expected);

    // The first display disconnects. The second one might be able to login as a display
    sockD.disconnectFromHost();
    QTest::qWait(delay);

    QCOMPARE(sockD.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(spyDisplayDisconnected.count(), 1);

    send = QString("%1bouh\n").arg(LOGIN_DISPLAY).toLatin1();
    expected = QString("%1\n").arg(OK).toLatin1();
    sockD2.write(send);

    QTest::qWait(delay);
    response = sockD2.readAll();
    QCOMPARE(response, expected);

    // A second player logs in, might be OK
    send = QString("%1bouh\n").arg(LOGIN_PLAYER).toLatin1();
    expected = QString("%1\n").arg(OK).toLatin1();
    sockP2.write(send);

    QTest::qWait(delay);
    response = sockP2.readAll();
    QCOMPARE(response, expected);

    // A third player wants to log in, but the limit is 2
    send = QString("%1bouh\n").arg(LOGIN_PLAYER).toLatin1();
    expected = QString("%1\n").arg(NO_MORE_ROOM).toLatin1();
    sockP3.write(send);

    QTest::qWait(delay);
    response = sockP3.readAll();
    QCOMPARE(response, expected);

    // The second player disconnects. The third player might now be able to log in
    sockP2.disconnectFromHost();
    QTest::qWait(delay);

    QCOMPARE(sockP2.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(spyPlayerDisconnected.count(), 1);

    send = QString("%1bouh\n").arg(LOGIN_PLAYER).toLatin1();
    expected = QString("%1\n").arg(OK).toLatin1();
    sockP3.write(send);

    QTest::qWait(delay);
    response = sockP3.readAll();
    QCOMPARE(response, expected);

    // End of login test
    sockP.close();
    sockP2.close();
    sockP3.close();
    sockD.close();
    sockD2.close();

    QTest::qWait(delay);
    QCOMPARE(sockP.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(sockP2.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(sockP3.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(sockD.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(sockD2.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(n.clientCount(), 0);
}

void ServerReceive::testFinished()
{
    QTcpSocket sockc;
    QTcpSocket * socks;
    QByteArray response, expected;

    sockc.connectToHost("127.0.0.1", 4242);
    QTest::qWait(delay);

    QCOMPARE(sockc.state(), QAbstractSocket::ConnectedState);
    QCOMPARE(n.clientCount(), 1);

    socks = n.clients().first();

    // Win send
    n.sendFinished(socks, true);
    QTest::qWait(delay);

    expected = QString("%1%2\n").arg(END_OF_GAME).arg('1').toLatin1();
    response = sockc.readAll();
    QCOMPARE(response, expected);

    // Lose send
    n.sendFinished(socks, false);
    QTest::qWait(delay);

    expected = QString("%1%2\n").arg(END_OF_GAME).arg('0').toLatin1();
    response = sockc.readAll();
    QCOMPARE(response, expected);
}

QTEST_GUILESS_MAIN(ServerReceive)


#include "tst_serverreceive.moc"
