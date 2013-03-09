#include <QString>
#include <QtTest>
#include <QTcpSocket>
#include <QDebug>

#include "../server/network.hpp"

class ServerReceive : public QObject
{
    Q_OBJECT
public:
    ServerReceive();
private Q_SLOTS:
    void testConnection();
    void initTestCase();
    void testLogin();

private:
    Network n;
};

ServerReceive::ServerReceive() :
    n(4242, 4, 1)
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

    sock.connectToHost(QString("127.0.0.1"), 4242, QAbstractSocket::ReadWrite, QAbstractSocket::IPv4Protocol);

    sock.waitForConnected();

    QCOMPARE(sock.state(), QAbstractSocket::ConnectedState);
    QCOMPARE(n.clientCount(), 1);
}

void ServerReceive::testLogin()
{
    QByteArray response, expected, send;
    QTcpSocket sock;

    sock.connectToHost("127.0.0.1", 4242);
    sock.waitForConnected(1000);

    send = QString("Abouh\n").toLatin1();
    expected = QString("a\n").toLatin1();
    sock.write(send);

    sock.waitForReadyRead(10000);
    response = sock.readAll();
    qDebug() << response;

    QCOMPARE(expected, response);

    sock.close();
}

QTEST_APPLESS_MAIN

(ServerReceive)

#include "tst_serverreceive.moc"
