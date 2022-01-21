#include "teachersignin.h"
#include "ui_TeacherSignIn.h"

TeacherSignIn::TeacherSignIn(QWidget *parent, QSettings* ptr, QSqlDatabase* DB):
    QDialog(parent),
    ui(new Ui::TeacherSignIn),
    sign_in(ptr),
    db(DB)
{
    ui->setupUi(this);

    connect(ui->SignInButton, &QPushButton::clicked, this, &TeacherSignIn::signin);

    info.load_SignInInfo(sign_in);
    ui->LoginLineEdit->insert(info.login);
    ui->PasswordLineEdit->insert(info.password);

    exec();
}

TeacherSignIn::~TeacherSignIn()
{
    delete ui;
}

void TeacherSignIn::signin(){
    ui->ErrorLabel->clear();
    info.login = ui->LoginLineEdit->text();
    info.password = ui->PasswordLineEdit->text();

    QSqlQuery* query = new QSqlQuery(*db);
    QSqlQueryModel *querymodel = new QSqlQueryModel;
    if (query->exec("SELECT name FROM teachers WHERE login = '" + info.login + "' AND password = '" + info.password + "';")) {
        querymodel->setQuery(*query);
    }
    QString name = querymodel->data(querymodel->index(0,0)).toString();
    if (name == "")
        ui->ErrorLabel->setText("Login or password was incorrect!");
    else {
        info.save_SignInInfo(sign_in);
        close();
    }
}

QString TeacherSignIn::get_login(){
    return info.login;
}

QString TeacherSignIn::get_password(){
    return info.password;
}
