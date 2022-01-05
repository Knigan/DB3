#include "teachersignin.h"
#include "ui_TeacherSignIn.h"

TeacherSignIn::TeacherSignIn(QWidget *parent):
    QDialog(parent),
    ui(new Ui::TeacherSignIn)
{
    ui->setupUi(this);
}

TeacherSignIn::~TeacherSignIn()
{
    delete ui;
}

QString TeacherSignIn::get_login() {
    return ui->nameLineEdit->text();
}

QString TeacherSignIn::get_password() {
    return ui->passwordLineEdit->text();
}

void TeacherSignIn::set_login(const QString& S) {
    ui->nameLineEdit->insert(S);
}

void TeacherSignIn::set_password(const QString& G) {
    ui->nameLineEdit->insert(G);
}
