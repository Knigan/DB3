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
};

#endif // TEACHERSIGNIN_H
