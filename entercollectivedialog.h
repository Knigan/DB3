#ifndef ENTERCOLLECTIVEDIALOG_H
#define ENTERCOLLECTIVEDIALOG_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlDatabase>

namespace Ui {
class EnterCollectiveDialog;
}

class EnterCollectiveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EnterCollectiveDialog(int id, QSqlDatabase* db = nullptr, QWidget *parent = nullptr);
    ~EnterCollectiveDialog();
    QString getName();

private:
    Ui::EnterCollectiveDialog *ui;

    QSqlDatabase* DB;
    int group_id;
};

#endif // ENTERCOLLECTIVEDIALOG_H
