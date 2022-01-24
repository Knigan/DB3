#ifndef TEACHERSIGNIN_H
#define TEACHERSIGNIN_H

#include <QDialog>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>

namespace Ui {
class TeacherSignIn;
}

struct TeacherSignInInfo {
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

class TeacherSignIn: public QDialog
{
    Q_OBJECT
public:
    explicit TeacherSignIn(QSqlDatabase* DB = nullptr, QSettings* ptr = nullptr, QWidget *parent = nullptr);
    QString get_login();
    QString get_password();
    ~TeacherSignIn();
    bool exit = true;

private:
    Ui::TeacherSignIn *ui;
    QSettings* sign_in;
    QSqlDatabase* db;
    TeacherSignInInfo info;

private slots:
    void signin();
};

#endif // TEACHERSIGNIN_H
