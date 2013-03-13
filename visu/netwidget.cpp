#include "netwidget.hpp"
#include "ui_netwidget.h"

#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QAbstractSocket>

NetWidget::NetWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetWidget)
{
    ui->setupUi(this);
    setWindowTitle("NetWidget");

    ui->labelState->setText("Disconnected");

    ui->address->setText("127.0.0.1");
    ui->port->setRange(1025, 65535);
    ui->port->setValue(4242);

    ui->buttonLogin->setEnabled(false);

    connect(ui->buttonConnect, SIGNAL(clicked()), this, SLOT(onConnectClicked()));
    connect(ui->buttonLogin, SIGNAL(clicked()), this, SLOT(onLoginClicked()));
}

NetWidget::~NetWidget()
{
    delete ui;
}


void NetWidget::onConnected()
{
    ui->labelState->setText("Connected");
    ui->buttonLogin->setEnabled(true);
}

void NetWidget::onDisconnected()
{
    ui->buttonConnect->setEnabled(true);
    ui->labelState->setText("Disconnected");
}

void NetWidget::onError()
{
    ui->buttonConnect->setEnabled(true);
    ui->labelState->setText("Error");
}

void NetWidget::onLogged()
{
    ui->labelState->setText("Logged");
}

void NetWidget::onCannotLogIn()
{
    ui->buttonLogin->setEnabled(true);

    ui->labelState->setText("Cannot log in");
}

void NetWidget::onConnectClicked()
{
    ui->buttonConnect->setEnabled(false);

    emit wantToConnectToHost(ui->address->text(), ui->port->value());
}

void NetWidget::onLoginClicked()
{
    ui->buttonLogin->setEnabled(false);

    emit wantToLogin();
}
