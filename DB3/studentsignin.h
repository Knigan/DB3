#ifndef STUDENTSIGNIN_H
#define STUDENTSIGNIN_H

#include <QDialog>

namespace Ui {
class StudentSignIn;
}

class StudentSignIn : public QDialog
{
    Q_OBJECT

public:

    explicit StudentSignIn(QWidget *parent = nullptr);
    ~StudentSignIn();
    QString get_surname();
    QString get_group();
    void set_surname(const QString&);
    void set_group(const QString&);

private:

    Ui::StudentSignIn *ui;
};


#endif // STUDENTSIGNIN_H
