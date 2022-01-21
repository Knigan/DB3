#include "StudentSignUp.h"
#include "ui_StudentSignUp.h"

SignUp::SignUp(QWidget *parent, QSqlDatabase* DB) :
    QDialog(parent),
    ui(new Ui::SignUp),
    db(DB)
{
    ui->setupUi(this);

    QSqlQuery* query = new QSqlQuery(*db);
    QSqlQueryModel* querymodel = new QSqlQueryModel;

    query->exec("SELECT COUNT(*) FROM groups");
    querymodel->setQuery(*query);
    int count = querymodel->data(querymodel->index(0, 0)).toInt();

    for (int i = 1; i <= count; ++i) {
        query->exec("SELECT name FROM groups WHERE id = " + QString::number(i) + ";");
        querymodel->setQuery(*query);
        QString str = querymodel->data(querymodel->index(0, 0)).toString();
        ui->comboBox->addItem(str);
    }

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
    QSqlQueryModel *querymodel = new QSqlQueryModel;

    if (query->exec("SELECT COUNT(*) FROM students WHERE login = '" + ui->LoginLineEdit->text() + "' AND password = '" + ui->PasswordLineEdit->text() + "';")) {
        querymodel->setQuery(*query);
    }
    int count = querymodel->data(querymodel->index(0,0)).toInt();

    if (count != 0)
        ui->ErrorLabel->setText("Login or password has already used!");
    else {
        if (query->exec("SELECT COUNT(*) FROM students WHERE surname = '" + ui->SurnameLineEdit->text() +
                        "' AND name = '" + ui->NameLineEdit->text() + "' AND group_id = '" + ui->comboBox->currentText() + "';")) {
            querymodel->setQuery(*query);
        }
        int count = querymodel->data(querymodel->index(0,0)).toInt();
        if (count != 0)
            ui->ErrorLabel->setText("This person has already signed up");
        else {

            if (query->exec("SELECT id FROM groups WHERE name = '" + ui->comboBox->currentText() + "';")) {
                querymodel->setQuery(*query);
            }
            int id = querymodel->data(querymodel->index(0,0)).toInt();

            query->prepare("INSERT INTO students(id, surname, name, group_id, team_id, login, password) VALUES (DEFAULT, :surname, :name, :group_id, 0, :login, :password);");
            query->bindValue(":surname", ui->SurnameLineEdit->text());
            query->bindValue(":name", ui->NameLineEdit->text());
            query->bindValue(":group_id", id);
            query->bindValue(":login", ui->LoginLineEdit->text());
            query->bindValue(":password", ui->PasswordLineEdit->text());
            query->exec();

            close();
        }
    }
}
