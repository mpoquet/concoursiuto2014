#include "widget.hpp"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QHostAddress>

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout * layout = new QVBoxLayout(this);
    setLayout(layout);

    _lineEdit = new QLineEdit(this);
    _textEdit = new QTextEdit(this);

    _textEdit->setReadOnly(true);

    layout->addWidget(_textEdit);
    layout->addWidget(_lineEdit);

    _socket = new QTcpSocket(this);

    connect(_socket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(_socket, SIGNAL(readyRead()), this, SLOT(onMessageReceived()));
}

void Widget::display(const QString &message)
{
    _textEdit->append(message);
}

void Widget::run(const QString &address, quint16 port)
{
    _socket->connectToHost(address, port);

    display(QString("Connecting to %1:%2...").arg(address).arg(port));
}

void Widget::sendMessage(const QByteArray &message)
{
    if (_socket->isWritable())
        _socket->write(message);
    else
        display("Cannot write socket");
}

void Widget::onConnected()
{
    display(QString("Connected to %1:%2").arg(_socket->peerAddress().toString()).arg(_socket->peerPort()));
}

void Widget::onDisconnected()
{
    display("Disconnected");
}

void Widget::onError(QAbstractSocket::SocketError e)
{
    qDebug() << e;
}

void Widget::onMessageReceived()
{
    QByteArray message = _socket->readAll();

    display(message);
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Return)
    {
        if (!_lineEdit->text().isEmpty())
        {
            display(QString("> %1").arg(_lineEdit->text()));

            QByteArray message = _lineEdit->text().toLatin1() + '\n';
            sendMessage(message);

            _lineEdit->clear();
        }
    }
}
