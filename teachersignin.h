#ifndef TEACHERSIGNIN_H
#define TEACHERSIGNIN_H

#include <QDialog>

namespace Ui {
class TeacherSignIn;
}


class TeacherSignIn: public QDialog
{
    Q_OBJECT
public:
    explicit TeacherSignIn(QWidget *parent = nullptr);
    ~TeacherSignIn();

private:
    Ui::TeacherSignIn *ui;
    QString get_login();
    QString get_password();
    void set_login(const QString& S);
    void set_password(const QString& G);
};

#endif // TEACHERSIGNIN_H
