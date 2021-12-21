#ifndef TEACHERDIALOG_H
#define TEACHERDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QVector>
#include <QDate>
#include <QDoubleValidator>

namespace Ui {
class TeacherDialog;
}

class TeacherDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::TeacherDialog *ui;

public:

    TeacherDialog(QDialog *parent = nullptr);
    ~TeacherDialog();
};

#endif
