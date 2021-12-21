#ifndef STUDENTDIALOG_H
#define STUDENTDIALOG_H

#include "StudentSignIn.h"

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
    StudentSignIn* m_studentSignIn_ui;
    QSqlDatabase m_db;
    Ui::StudentDialog* m_ui;
    QSettings* m_settings;

};


#endif // STUDENTDIALOG_H
