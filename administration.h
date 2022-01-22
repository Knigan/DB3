#ifndef ADMINISTRATION_H
#define ADMINISTRATION_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QComboBox>

namespace Ui {
class Administration;
}

class Administration : public QDialog
{
    Q_OBJECT

public:
    explicit Administration(QSqlDatabase* DB = nullptr, QWidget *parent = nullptr);
    ~Administration();

private slots:
    void addObject();
    void removeObject();
    void removeStudent();
    void removeTeacher();
    void assignObject();
    void createGroup();

private:
    Ui::Administration *ui;
    QSqlDatabase* db;
    void combobox_work(const QString& table, const QString& field, QComboBox* Box);
    QSqlQueryModel* makeQuery(const QString&);
    void refresh();
};

#endif // ADMINISTRATION_H
