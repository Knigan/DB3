#ifndef TEACHERDIALOG_H
#define TEACHERDIALOG_H

#include "teachersignin.h"
#include "editprofiledialog.h"

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSettings>
#include <QStringList>
#include <QVector>
#include <QMessageBox>
#include <QDate>
#include <QDoubleValidator>

namespace Ui {
class TeacherDialog;
}

class TeacherDialog : public QDialog
{
    Q_OBJECT

struct TeacherInfo {
    QString login;
    QString password;
    QString name;
    QString subjects[5];
    int id;

    TeacherInfo():
        login(""),
        password(""),
        id(0),
        name(""),
        subjects{"","","","",""}
    {}
};

private slots:
    void exit();
    void editProfile();

private:
    QSqlDatabase* m_db;
    Ui::TeacherDialog *m_ui;
    QSettings *m_settings;
    TeacherInfo m_info;

    QSqlQueryModel* makeQuery(const QString& queryString);
    void save_TeacherInfo(const TeacherInfo& info);

public:

    TeacherDialog(QDialog *parent = nullptr, QSqlDatabase* p = nullptr);
    ~TeacherDialog();
};

#endif
