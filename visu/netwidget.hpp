#ifndef NETWIDGET_HPP
#define NETWIDGET_HPP

#include <QWidget>

namespace Ui {
class NetWidget;
}

class NetWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit NetWidget(QWidget *parent = 0);
    ~NetWidget();

signals:
    void wantToConnectToHost(QString address, quint16 port);
    void wantToLogin();

public slots:
    void onConnected();
    void onDisconnected();
    void onError();
    void onLogged();
    void onCannotLogIn();

private slots:
    void onConnectClicked();
    void onLoginClicked();
    
private:
    Ui::NetWidget *ui;
};

#endif // NETWIDGET_HPP
