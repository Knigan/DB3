#include "createcollectivedialog.h"
#include "ui_createcollectivedialog.h"

CreateCollectiveDialog::CreateCollectiveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateCollectiveDialog)
{
    ui->setupUi(this);
}

CreateCollectiveDialog::~CreateCollectiveDialog()
{
    delete ui;
}
