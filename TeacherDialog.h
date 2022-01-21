#ifndef TEACHERDIALOG_H
#define TEACHERDIALOG_H

#include "teachersignin.h"
#include "editprofiledialog.h"

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "teachersignin.h"

#include <QSettings>
#include <QStringList>
#include <QVector>
#include <QMessageBox>
#include <QDate>
#include <QDoubleValidator>
#include <QAbstractItemModel>
#include <QErrorMessage>
#include <QDebug>

namespace Ui {
class TeacherDialog;
}

class TeacherDialog : public QDialog
{
    Q_OBJECT

struct TeacherInfo{
    QString login;
    QString password;
    int id;
    int subjectId;
    QString name;
    QString surname;

    TeacherInfo():
        login(""),
        password(""),
        id(0),
        subjectId(0),
        name(""),
        surname("")
    {}
};


private slots:
    void exit();
    void editProfile();
    void giveTask();
    void signUpTeacher();

private:
    QSqlDatabase* m_db;
    Ui::TeacherDialog *m_ui;
    QSettings *m_settings;
    TeacherInfo m_info;

    QSqlQueryModel* makeQuery(const QString& queryString);
    void save_TeacherInfo(const TeacherInfo& info);
    void load_TeacherInfo(TeacherInfo& info);

public:

    TeacherDialog(QDialog *parent = nullptr, QSqlDatabase* p = nullptr);
    ~TeacherDialog();
};

#endif
