#include "AddMemberDialog.h"
#include "ui_AddMemberDialog.h"


AddMemberDialog::AddMemberDialog(QWidget *parent)
    :QDialog(parent),
    ui(new Ui::AddMemberDialog)
{
    ui->setupUi(this);

    connect(ui->addButton   , &QPushButton::clicked,this, &AddMemberDialog::addMember);
    connect(ui->cancelButton, &QPushButton::clicked,this, &AddMemberDialog::cancel);
}

AddMemberDialog::~AddMemberDialog()
{

}

void AddMemberDialog::addMember()
{

}

void AddMemberDialog::cancel()
{
   delete ui;
}
