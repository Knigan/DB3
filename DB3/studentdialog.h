#ifndef STUDENTDIALOG_H
#define STUDENTDIALOG_H

#include <QDialog>
#include "signin.h"
#include <QSettings>

namespace Ui {
class StudentDialog;
}

class StudentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StudentDialog(QWidget *parent = nullptr);
    ~StudentDialog();

private:
    Ui::StudentDialog *ui;
    QSettings* sign_in;
};

#endif // STUDENTDIALOG_H
