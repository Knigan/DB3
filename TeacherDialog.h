#ifndef TEACHERDIALOG_H
#define TEACHERDIALOG_H

#include "teachersignin.h"

#include <QDialog>
#include <QSettings>
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

    QSettings *m_settings;
public:

    TeacherDialog(QDialog *parent = nullptr);
    ~TeacherDialog();
};

#endif
