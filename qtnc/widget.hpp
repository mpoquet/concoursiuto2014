#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QTcpSocket>
#include <QKeyEvent>
#include <QAbstractSocket>

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);

    void display(const QString & message);
    void run(const QString & address, quint16 port);

private:
    void sendMessage(const QByteArray &message);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError e);
    void onMessageReceived();

    void keyPressEvent(QKeyEvent * e);

private:
    QTextEdit * _textEdit;
    QLineEdit * _lineEdit;
    QTcpSocket * _socket;
};

#endif // WIDGET_HPP
