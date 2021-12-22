#include "deletememberdialog.h"
#include "ui_deletememberdialog.h"

deleteMemberDialog::deleteMemberDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::deleteMemberDialog)
{
    ui->setupUi(this);
    connect(ui->excludeButton, &QPushButton::clicked, this, &deleteMemberDialog::deleteMember);
}

deleteMemberDialog::~deleteMemberDialog()
{
    delete ui;
}

void deleteMemberDialog::deleteMember()
{

}
