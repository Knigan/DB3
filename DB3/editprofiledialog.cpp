#include "editprofiledialog.h"
#include "ui_editprofiledialog.h"

EditProfileDialog::EditProfileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditProfileDialog)
{
    ui->setupUi(this);

    connect(ui->changeButton, &QPushButton::clicked,this, &EditProfileDialog::change);
    connect(ui->cancelButton, &QPushButton::clicked,this, &EditProfileDialog::cancel);
}

EditProfileDialog::~EditProfileDialog()
{
    delete ui;
}

void EditProfileDialog::change()
{

}

void EditProfileDialog::cancel()
{

}
