#include "SignIn.h"
#include "ui_SignIn.h"

SignIn::SignIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignIn)
{
    ui->setupUi(this);
}

SignIn::~SignIn()
{
    delete ui;
}

QString SignIn::get_login() {
    return ui->LoginLineEdit->text();
}

QString SignIn::get_password() {
    return ui->PasswordLineEdit->text();
}

void SignIn::set_login(const QString& S) {
    ui->LoginLineEdit->insert(S);
}

void SignIn::set_password(const QString& G) {
    ui->PasswordLineEdit->insert(G);
}

