#include "entercollectivedialog.h"
#include "ui_entercollectivedialog.h"

EnterCollectiveDialog::EnterCollectiveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterCollectiveDialog)
{
    ui->setupUi(this);

    connect(ui->enterButton, &QPushButton::clicked, this, &EnterCollectiveDialog::enter);
    connect(ui->cancelButton, &QPushButton::clicked, this, &EnterCollectiveDialog::cancel);
}

EnterCollectiveDialog::~EnterCollectiveDialog()
{
    delete ui;
}

void EnterCollectiveDialog::enter()
{

}

void EnterCollectiveDialog::cancel()
{

}
