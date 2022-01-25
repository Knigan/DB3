#include "entercollectivedialog.h"
#include "ui_entercollectivedialog.h"

EnterCollectiveDialog::EnterCollectiveDialog(int id, QSqlDatabase* db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterCollectiveDialog),
    DB(db),
    group_id(id)
{
    ui->setupUi(this);

    QSqlQuery* query = new QSqlQuery(*DB);
    QSqlQueryModel* querymodel = new QSqlQueryModel;

    query->exec("SELECT COUNT(*) FROM teams WHERE id != 0 AND check_group(id, " + QString::number(group_id) + ");");
    querymodel->setQuery(*query);
    int count = querymodel->data(querymodel->index(0, 0)).toInt();

    for (int i = 1; i <= count; ++i) {
        query->exec("SELECT name FROM teams WHERE id = " + QString::number(i) + " AND check_group(id, " + QString::number(group_id) + ");");
        querymodel->setQuery(*query);
        QString str = querymodel->data(querymodel->index(0, 0)).toString();
        ui->comboBox->addItem(str);
    }

}

EnterCollectiveDialog::~EnterCollectiveDialog()
{
    delete ui;
}

QString EnterCollectiveDialog::getName() {
    return ui->comboBox->currentText();
}
