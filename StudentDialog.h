#ifndef STUDENTDIALOG_H
#define STUDENTDIALOG_H

#include "StudentSignIn.h"
#include "editprofiledialog.h"
#include "createcollectivedialog.h"
#include "entercollectivedialog.h"

#include <QBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QDialog>
#include <QMessageBox>
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
    QString login;
    QString password;
    int id;
    int teamId;
    QString name;
    QString surname;
    int groupId;

    StudentInfo():
        login(""),
        password(""),
        id(0),
        teamId(0),
        name(""),
        surname(""),
        groupId(0)
    {}
};


class StudentDialog : public QDialog
{
    Q_OBJECT

public:

    StudentDialog(QDialog *parent = nullptr, QSqlDatabase* p = nullptr);
    ~StudentDialog();

private slots:

    void editProfile();
    void leaveCollective();
    void exit();
    void refresh();
    void createCollective();
    void enterCollective();
    void acceptEntering();
    void declineEntering();

private:

    QSqlDatabase* m_db;
    Ui::StudentDialog* m_ui;
    QSettings* m_settings;
    StudentInfo m_info;

    void save_StudentInfo(const StudentInfo&);
    void load_StudentInfo(StudentInfo&);

    bool isUniqueCollectiveName(const QString&);

    QWidget* createAcceptButtonWidget();
    QWidget* createDeclineButtonWidget();
    QSqlQueryModel* makeQuery(const QString& queryString);
    void refreshCollectiveInfo();
    void refreshRequests();
    void refreshLabs();

};


#endif // STUDENTDIALOG_H
