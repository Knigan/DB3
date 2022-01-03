#ifndef SIGNIN_H
#define SIGNIN_H

#include <QDialog>
#include <QSettings>

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
    explicit SignIn(QWidget *parent = nullptr);
    ~SignIn();
    QString get_login();
    QString get_password();
    void set_login(const QString&);
    void set_password(const QString&);

private:
    Ui::SignIn *ui;
};

#endif // SIGNIN_H
