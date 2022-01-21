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
    explicit TeacherSignIn(QWidget *parent = nullptr, QSettings* ptr = nullptr, QSqlDatabase* DB = nullptr);
    QString get_login();
    QString get_password();
    ~TeacherSignIn();

private:
    Ui::TeacherSignIn *ui;
    QSettings* sign_in;
    QSqlDatabase* db;
    TeacherSignInInfo info;

private slots:
    void signin();
};

#endif // TEACHERSIGNIN_H
