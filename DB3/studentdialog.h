#ifndef STUDENTDIALOG_H
#define STUDENTDIALOG_H

#include <QDialog>
#include "studentsignin.h"
#include <QSettings>

namespace Ui {
class StudentDialog;
}

struct StudentInfo {
    QString surname;
    QString group;
};

class StudentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StudentDialog(QWidget *parent = nullptr);
    ~StudentDialog();

private:
    Ui::StudentDialog *ui;
    QSettings* sign_in;
    void save_StudentInfo(const StudentInfo&);
    void load_StudentInfo(StudentInfo&);
};

#endif // STUDENTDIALOG_H
