#include "studentdialog.h"
#include "ui_studentdialog.h"

StudentDialog::StudentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StudentDialog)
{
    ui->setupUi(this);
    sign_in = new QSettings("SignIn_config.ini", QSettings::IniFormat, this);
    SignIn S;
    SignInInfo info;
    info.load_SignInInfo(sign_in);
    S.set_login(info.login);
    S.set_password(info.password);
    if (S.exec() == QDialog::Accepted) {
        info.login = S.get_login();
        info.password = S.get_password();
        info.save_SignInInfo(sign_in);
        exec();
    }
}

StudentDialog::~StudentDialog()
{
    delete ui;
}
