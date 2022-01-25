#include "administration.h"
#include "ui_administration.h"

Administration::Administration(QSqlDatabase* DB, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Administration),
    db(DB)
{
    ui->setupUi(this);

    connect(ui->AddObjectButton,     &QPushButton::clicked, this, &Administration::addObject);
    connect(ui->RemoveObjectButton,  &QPushButton::clicked, this, &Administration::removeObject);
    connect(ui->RemoveStudentButton, &QPushButton::clicked, this, &Administration::removeStudent);
    connect(ui->RemoveTeacherButton, &QPushButton::clicked, this, &Administration::removeTeacher);
    connect(ui->AssignObject,        &QPushButton::clicked, this, &Administration::assignObject);
    connect(ui->GroupButton,         &QPushButton::clicked, this, &Administration::createGroup);
    connect(ui->SetTeacherButton,    &QPushButton::clicked, this, &Administration::setTeacher);


    refresh();
    ui->ErrorLabel->clear();

    connect(ui->StudentGroupComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](){
        ui->StudentSurnameComboBox->clear();
        QSqlQuery* query = new QSqlQuery(*db);
        QSqlQueryModel* querymodel = new QSqlQueryModel;

        querymodel = makeQuery("SELECT id FROM groups WHERE name = '" + ui->StudentGroupComboBox->currentText() + "';");
        int group_id = querymodel->data(querymodel->index(0, 0)).toInt();

        query->exec("SELECT COUNT(*) FROM students WHERE group_id = " + QString::number(group_id) + ";");
        querymodel->setQuery(*query);
        int count = querymodel->data(querymodel->index(0, 0)).toInt();

        int k = 1;
        query->exec("SELECT surname FROM students WHERE group_id = " + QString::number(group_id) + ";");
        while(query->next() || k <= count) {
            QString str = query->value(0).toString();
            if (str != "")
            {
                ui->StudentSurnameComboBox->addItem(str);
                ++k;
            }
        }
    });

    connect(ui->StudentSurnameComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](){
        ui->StudentNameComboBox->clear();
        QSqlQuery* query = new QSqlQuery(*db);
        QSqlQueryModel* querymodel = new QSqlQueryModel;

        querymodel = makeQuery("SELECT id FROM groups WHERE name = '" + ui->StudentGroupComboBox->currentText() + "';");
        int group_id = querymodel->data(querymodel->index(0, 0)).toInt();

        query->exec("SELECT COUNT(*) FROM students WHERE group_id = " + QString::number(group_id) + " AND surname = '"
                    + ui->StudentSurnameComboBox->currentText() + "';");
        querymodel->setQuery(*query);
        int count = querymodel->data(querymodel->index(0, 0)).toInt();

        int k = 1;
        query->exec("SELECT name FROM students WHERE group_id = " + QString::number(group_id) + " AND surname = '"
                                          + ui->StudentSurnameComboBox->currentText() + "';");
        while(query->next() || k <= count) {
            QString str = query->value(0).toString();
            if (str != "")
            {
                ui->StudentNameComboBox->addItem(str);
                ++k;
            }
        }
    });

    connect(ui->TeacherSurnameComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this]() {
        ui->TeacherNameComboBox->clear();
        QSqlQuery* query = new QSqlQuery(*db);
        QSqlQueryModel* querymodel = new QSqlQueryModel;

        query->exec("SELECT COUNT(*) FROM teachers WHERE surname = '" + ui->TeacherSurnameComboBox->currentText() + "';");
        querymodel->setQuery(*query);
        int count = querymodel->data(querymodel->index(0, 0)).toInt();

        int k = 1;
        query->exec("SELECT name FROM teachers WHERE surname = '" + ui->TeacherSurnameComboBox->currentText() + "';");

        while(query->next() || k <= count) {
            QString str = query->value(0).toString();
            if (str != "")
            {
                ui->TeacherNameComboBox->addItem(str);
                ++k;
            }
        }
    });

    exec();
}

Administration::~Administration()
{
    delete ui;
}

void Administration::refresh() {
    ui->ErrorLabel->clear();

    combobox_work("objects", "name", ui->RemoveObjectComboBox);
    combobox_work("groups", "name", ui->GroupComboBox);
    combobox_work("objects", "name", ui->ObjectComboBox);
    combobox_work("groups", "name", ui->StudentGroupComboBox);
    combobox_work("objects", "name", ui->TeacherObjectComboBox);
    combobox_work("teachers", "surname", ui->TeacherSurnameComboBox);

    ui->ErrorLabel->setText("Успешно!");
}

void Administration::combobox_work(const QString& table, const QString& field, QComboBox* Box) {
    Box->clear();
    QSqlQuery* query = new QSqlQuery(*db);
    QSqlQueryModel* querymodel = new QSqlQueryModel;
    query->exec("SELECT COUNT(*) FROM " + table + ";");
    querymodel->setQuery(*query);
    int count = querymodel->data(querymodel->index(0, 0)).toInt();

    int k = 1;
    query->exec("SELECT " + field + " FROM " + table + ";");

    while(query->next() || k <= count) {
        QString str = query->value(0).toString();
        if (str != "")
        {
            Box->addItem(str);
            ++k;
        }
    }
}

QSqlQueryModel* Administration::makeQuery(const QString& queryString)
{
    QSqlQuery* query = new QSqlQuery(*db);
    QSqlQueryModel *querymodel = new QSqlQueryModel;
    if (query->exec(queryString))
    {
        querymodel->setQuery(*query);
    }
    return querymodel;
}

void Administration::addObject() {
    QSqlQuery* query = new QSqlQuery(*db);

    QSqlQueryModel* querymodel = new QSqlQueryModel;
    QString str = ui->AddObjectLineEdit->text();

    querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + str + "';");
    if (querymodel->rowCount() != 0)
        ui->ErrorLabel->setText("Этот предмет уже существует!");
    else {
        if (str != "") {
            query->exec("INSERT INTO objects VALUES (DEFAULT, '" + str + "');");
            refresh();
        }
        else
            ui->ErrorLabel->setText("Введите название предмета!");
    }
}

void Administration::removeObject() {
    QSqlQuery* query = new QSqlQuery(*db);
    query->exec("DELETE FROM objects WHERE name = '" + ui->RemoveObjectComboBox->currentText() + "';");
    refresh();
}

void Administration::removeStudent() {
    QSqlQuery* query = new QSqlQuery(*db);
    QSqlQueryModel* querymodel = new QSqlQueryModel;

    querymodel = makeQuery("SELECT id FROM groups WHERE name = '" + ui->StudentGroupComboBox->currentText() + "';");
    int group_id = querymodel->data(querymodel->index(0, 0)).toInt();

    querymodel = makeQuery("SELECT * FROM students WHERE surname = '" + ui->StudentSurnameComboBox->currentText()
                           + "' AND name = '" + ui->StudentNameComboBox->currentText() + "' AND group_id = "
                           + QString::number(group_id) + ";");
    if (querymodel->rowCount() == 0) {
        ui->ErrorLabel->setText("Такого студента нет в выбранной группе!");
    }
    else {
        query->exec("DELETE FROM students WHERE surname = '" + ui->StudentSurnameComboBox->currentText()
                    + "' AND name = '" + ui->StudentNameComboBox->currentText() + "' AND group_id = "
                    + QString::number(group_id) + ";");
        refresh();
    }

}

void Administration::removeTeacher() {
    QSqlQuery* query = new QSqlQuery(*db);

    QSqlQueryModel* querymodel = new QSqlQueryModel;

    querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + ui->TeacherObjectComboBox->currentText() + "';");
    int object_id = querymodel->data(querymodel->index(0, 0)).toInt();

    querymodel = makeQuery("SELECT id FROM teachers WHERE surname = '" + ui->TeacherSurnameComboBox->currentText()
                           + "' AND name = '" + ui->TeacherNameComboBox->currentText()
                           + "' AND object = " + QString::number(object_id) + ";");
    if (querymodel->rowCount() == 0) {
        ui->ErrorLabel->setText("Этого преподавателя не существует, или он не ведёт выбранный предмет!");
    }
    else {
        int id = querymodel->data(querymodel->index(0, 0)).toInt();

        if (id != 1) {
            query->exec("DELETE FROM teachers WHERE surname = '" + ui->TeacherSurnameComboBox->currentText()
                        + "' AND name = '" + ui->TeacherNameComboBox->currentText()
                        + "' AND object = " + QString::number(object_id) + ";");
            refresh();
        }
        else
            ui->ErrorLabel->setText("Нельзя удалить старшего преподавателя!");

    }
}

void Administration::setTeacher() {
    QSqlQuery* query = new QSqlQuery(*db);

    QSqlQueryModel* querymodel = new QSqlQueryModel;

    querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + ui->TeacherObjectComboBox->currentText() + "';");
    int object_id = querymodel->data(querymodel->index(0, 0)).toInt();

    querymodel = makeQuery("SELECT * FROM teachers WHERE surname = '" + ui->TeacherSurnameComboBox->currentText()
                           + "' AND name = '" + ui->TeacherNameComboBox->currentText()
                           + "' AND object = " + QString::number(object_id) + ";");

    if (querymodel->rowCount() != 0)
        ui->ErrorLabel->setText("Этот преподаватель уже ведёт этот предмет!");
    else {
        querymodel = makeQuery("SELECT login, password FROM teachers WHERE surname = '" + ui->TeacherSurnameComboBox->currentText()
                               + "' AND name = '" + ui->TeacherNameComboBox->currentText() + "';");
        QString login = querymodel->data(querymodel->index(0, 0)).toString();
        QString password = querymodel->data(querymodel->index(0, 1)).toString();

        query->exec("INSERT INTO teachers VALUES(DEFAULT, '" + ui->TeacherSurnameComboBox->currentText()
                    + "', '" + ui->TeacherNameComboBox->currentText()
                    + "', " + QString::number(object_id)
                    + ", '" + login + "', '" + password + "');");

        refresh();
    }
}

void Administration::assignObject() {
    QSqlQuery* query = new QSqlQuery(*db);
    QSqlQueryModel* querymodel = new QSqlQueryModel;

    querymodel = makeQuery("SELECT id FROM groups WHERE name = '" + ui->GroupComboBox->currentText() + "';");
    int group_id = querymodel->data(querymodel->index(0, 0)).toInt();

    querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + ui->ObjectComboBox->currentText() + "';");
    int object_id = querymodel->data(querymodel->index(0, 0)).toInt();

    querymodel = makeQuery("SELECT * FROM groups_and_objects WHERE group_id = " + QString::number(group_id) + " AND object_id = " + QString::number(object_id) + ";");
    if (querymodel->rowCount() != 0)
        ui->ErrorLabel->setText("Эта группа уже изучает выбранный предмет!");
    else {
        query->exec("INSERT INTO groups_and_objects VALUES (" + QString::number(group_id) + ", " + QString::number(object_id) + ");");
        refresh();
    }
}

void Administration::createGroup() {
    QSqlQuery* query = new QSqlQuery(*db);

    QSqlQueryModel* querymodel = new QSqlQueryModel;
    QString str = ui->GroupLineEdit->text();

    querymodel = makeQuery("SELECT id FROM groups WHERE name = '" + str + "';");
    if (querymodel->rowCount() != 0)
        ui->ErrorLabel->setText("Такая группа уже существует!");
    else {
        if (str != "") {
            query->exec("INSERT INTO groups VALUES (DEFAULT, '" + str + "');");
            refresh();
        }
        else
            ui->ErrorLabel->setText("Введите имя группы!");
    }
}
