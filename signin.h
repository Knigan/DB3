#ifndef SIGNIN_H
#define SIGNIN_H

#include <QDialog>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "signup.h"

namespace Ui {
class SignIn;
}

struct SignInInfo {
    QString login;
    QString password;
    void save_SignInInfo(QSettings* sign_in) {
        sign_in->setValue("Login", login);
        sign_in->setValue("Password", password);
    }
    void load_SignInInfo(QSettings* sign_in) {
        login = sign_in->value("Login").toString();
        password = sign_in->value("Password").toString();
    }
};

class SignIn : public QDialog
{
    Q_OBJECT

public:
    explicit SignIn(QWidget *parent = nullptr, QSettings* ptr = nullptr, QSqlDatabase* DB = nullptr);
    ~SignIn();

private:
    Ui::SignIn *ui;
    QSettings* sign_in;
    QSqlDatabase* db;
    SignInInfo info;
    void signin();
    void signup();
};

#endif // SIGNIN_H
