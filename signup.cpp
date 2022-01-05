#include "signup.h"
#include "ui_signup.h"

SignUp::SignUp(QWidget *parent, QSqlDatabase* DB) :
    QDialog(parent),
    ui(new Ui::SignUp),
    db(DB)
{
    ui->setupUi(this);
    connect(ui->SignUpButton, &QPushButton::clicked, this, &SignUp::signup);
    exec();
}

SignUp::~SignUp()
{
    delete ui;
}

void SignUp::signup()
{
    QSqlQuery* query = new QSqlQuery(*db);

    query->prepare("INSERT INTO students(id, surname, name, grp, team_id, login, password) VALUES (DEFAULT, :surname, :name, :grp, 0, :login, :password);");
    query->bindValue(":surname", ui->SurnameLineEdit->text());
    query->bindValue(":name", ui->NameLineEdit->text());
    query->bindValue(":grp", ui->GroupLineEdit->text());
    query->bindValue(":login", ui->LoginLineEdit->text());
    query->bindValue(":password", ui->PasswordLineEdit->text());
    query->exec();

    QSqlQueryModel *querymodel = new QSqlQueryModel;
    if (query->exec("SELECT COUNT(*) FROM students WHERE team_id = 0;"))
    {
        querymodel->setQuery(*query);
    }
    int count = querymodel->data(querymodel->index(0,0)).toInt();
    query->prepare("UPDATE teams SET count_of_students = :count WHERE id = 0;");
    query->bindValue(":count", count);
    query->exec();

    this->close();
}
