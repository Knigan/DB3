#include "studentdialog.h"
#include "ui_studentdialog.h"

StudentDialog::StudentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StudentDialog)
{
    ui->setupUi(this);
    sign_in = new QSettings("connection_config.ini", QSettings::IniFormat, this);
    StudentSignIn S;
    StudentInfo info;
    load_StudentInfo(info);
    S.set_surname(info.surname);
    S.set_group(info.group);
    if (S.exec() == QDialog::Accepted) {
        info.surname = S.get_surname();
        info.group = S.get_group();
        save_StudentInfo(info);
    }
    exec();
}

StudentDialog::~StudentDialog()
{
    delete ui;
}

void StudentDialog::save_StudentInfo(const StudentInfo& info) {
    sign_in->setValue("Surname", info.surname);
    sign_in->setValue("Group", info.group);
}

void StudentDialog::load_StudentInfo(StudentInfo& info) {
    info.surname = sign_in->value("Surname").toString();
    info.group = sign_in->value("Group").toString();
}
