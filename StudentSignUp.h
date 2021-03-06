#ifndef STUDENTSIGNUP_H
#define STUDENTSIGNUP_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>

namespace Ui {
class SignUp;
}

class SignUp : public QDialog
{
    Q_OBJECT

public:
    explicit SignUp(QWidget *parent = nullptr, QSqlDatabase* DB = nullptr);
    ~SignUp();

private:
    Ui::SignUp *ui;
    QSqlDatabase* db;
    void signup();
};

#endif // STUDENTSIGNUP_H
