#ifndef STUDENTDIALOG_H
#define STUDENTDIALOG_H

#include "StudentSignIn.h"
#include "AddMemberDialog.h"
#include "editprofiledialog.h"
#include "deletememberdialog.h"

#include <QDialog>
#include <QDialog>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QDebug>
#include <QStringListModel>
#include <QSettings>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class StudentDialog; }
QT_END_NAMESPACE

struct StudentInfo
{
    QString surname;
    QString group;
};

class StudentDialog : public QDialog
{
    Q_OBJECT

public:

    StudentDialog(QDialog *parent = nullptr);
    ~StudentDialog();

private slots:

    void editProfile();
    void addNewMember();
    void deleteMember();
    void deleteCollective();
    void exit();

private:

    //void MakeQuery(const QString& a_queryString);

    QSqlDatabase m_db;
    Ui::StudentDialog* m_ui;
    QSettings* m_settings;

    void save_StudentInfo(const StudentInfo&);
    void load_StudentInfo(StudentInfo&);


};


#endif // STUDENTDIALOG_H
