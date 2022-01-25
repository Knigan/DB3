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

    query->exec("SELECT COUNT(*) FROM teams WHERE id != 0 AND check_team(id, " + QString::number(group_id) + ");");
    querymodel->setQuery(*query);
    int count = querymodel->data(querymodel->index(0, 0)).toInt();

    int k = 1;
    query->exec("SELECT name FROM teams WHERE check_team(id, " + QString::number(group_id) + ");");

    while(query->next() || k <= count) {
        QString str = query->value(0).toString();
        if (str != "")
        {
            ui->comboBox->addItem(str);
            ++k;
        }
    }

}

EnterCollectiveDialog::~EnterCollectiveDialog()
{
    delete ui;
}

QString EnterCollectiveDialog::getName() {
    return ui->comboBox->currentText();
}
