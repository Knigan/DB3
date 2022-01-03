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
