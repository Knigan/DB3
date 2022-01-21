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
    explicit EnterCollectiveDialog(QWidget *parent = nullptr, QSqlDatabase* db = nullptr);
    ~EnterCollectiveDialog();
    QString getName();

private:
    Ui::EnterCollectiveDialog *ui;

    QSqlDatabase* DB;
};

#endif // ENTERCOLLECTIVEDIALOG_H
