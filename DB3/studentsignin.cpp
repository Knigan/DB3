#include "studentsignin.h"
#include "ui_studentsignin.h"

StudentSignIn::StudentSignIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StudentSignIn)
{
    ui->setupUi(this);
}

StudentSignIn::~StudentSignIn()
{
    delete ui;
}

QString StudentSignIn::get_surname() {
    return ui->SurnameLineEdit->text();
}

QString StudentSignIn::get_group() {
    return ui->GroupLineEdit->text();
}

void StudentSignIn::set_surname(const QString& S) {
    ui->SurnameLineEdit->insert(S);
}

void StudentSignIn::set_group(const QString& G) {
    ui->GroupLineEdit->insert(G);
}

