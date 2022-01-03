#include "entercollectivedialog.h"
#include "ui_entercollectivedialog.h"

EnterCollectiveDialog::EnterCollectiveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterCollectiveDialog)
{
    ui->setupUi(this);
}

EnterCollectiveDialog::~EnterCollectiveDialog()
{
    delete ui;
}
