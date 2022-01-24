#include "StudentSignIn.h"
#include "ui_StudentSignIn.h"

SignIn::SignIn(QSqlDatabase* DB, QSettings* ptr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignIn),
    sign_in(ptr),
    db(DB)
{
    ui->setupUi(this);
    connect(ui->SignInButton, &QPushButton::clicked, this, &SignIn::signin);
    connect(ui->SignUpButton, &QPushButton::clicked, this, &SignIn::signup);

    info.load_SignInInfo(sign_in);
    ui->LoginLineEdit->insert(info.login);
    ui->PasswordLineEdit->insert(info.password);

    exec();
}

SignIn::~SignIn()
{
    delete ui;
}

void SignIn::signin() {
    ui->ErrorLabel->clear();
    SignInInfo info;
    info.login = ui->LoginLineEdit->text();
    info.password = ui->PasswordLineEdit->text();

    QSqlQuery* query = new QSqlQuery(*db);
    QSqlQueryModel *querymodel = new QSqlQueryModel;
    if (query->exec("SELECT name FROM students WHERE login = '" + info.login + "' AND password = '" + info.password + "';")) {
        querymodel->setQuery(*query);
    }
    QString name = querymodel->data(querymodel->index(0,0)).toString();
    if (name == "")
        ui->ErrorLabel->setText("Login or password was incorrect!");
    else {
        info.save_SignInInfo(sign_in);
        exit = false;
        close();
    }
}

void SignIn::signup() {
    SignUp S(nullptr, db);
}
