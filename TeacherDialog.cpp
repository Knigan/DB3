#include "TeacherDialog.h"
#include "ui_TeacherDialog.h"

TeacherDialog::TeacherDialog(QDialog *parent, QSqlDatabase* p) :
    QDialog(parent),
    m_db(p),
    m_ui(new Ui::TeacherDialog)
{
    m_ui->setupUi(this);

    connect(m_ui->exitButton,         &QPushButton::clicked, this, &TeacherDialog::exit);
    connect(m_ui->signUpButton,       &QPushButton::clicked, this, &TeacherDialog::signUpTeacher);
    connect(m_ui->editProfileButton,  &QPushButton::clicked, this, &TeacherDialog::editProfile);
    connect(m_ui->sendPushButton,     &QPushButton::clicked, this, &TeacherDialog::giveTask);
    connect(m_ui->RefreshButton,      &QPushButton::clicked, this, &TeacherDialog::refresh);
    connect(m_ui->RefreshButton2,     &QPushButton::clicked, this, &TeacherDialog::refresh);
    connect(m_ui->AddTaskButton,      &QPushButton::clicked, this, &TeacherDialog::addTask);
    connect(m_ui->SetTaskButton,      &QPushButton::clicked, this, &TeacherDialog::setTask);
    connect(m_ui->AcceptTaskButton,   &QPushButton::clicked, this, &TeacherDialog::acceptTask);
    connect(m_ui->allGroupsListView,  &QListView::clicked,   this, [this](const QModelIndex& index){
                QSqlQueryModel* querymodel = makeQuery("SELECT DISTINCT teams.team from groups join (SELECT students.group_id as group, teams.name as team "
                                                       "FROM students JOIN teams ON students.team_id = teams.id) AS teams ON teams.group = groups.id "
                                                       "WHERE groups.name = '" + index.data().toString() + "';");
                m_ui->collectivesListView->setModel(querymodel);
    });

    connect(m_ui->collectivesListView, &QListView::clicked, this, [this](const QModelIndex& index){
                QSqlQueryModel* querymodel = makeQuery("select concat(students.surname, ' ', students.name) from teams join students "
                                                       "on students.team_id = teams.id where teams.name = '" + index.data().toString() + "' order by students.surname");
                m_ui->studentsListView->setModel(querymodel);
                querymodel = makeQuery("SELECT DISTINCT name FROM labs WHERE object = " + QString::number(m_info.subjectId) + ";");
                m_ui->labsListView->setModel(querymodel);
    });

    connect(m_ui->CreateTaskComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this]() {
        QSqlQuery* query = new QSqlQuery(*m_db);
        QSqlQueryModel* querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + m_ui->CreateTaskComboBox->currentText() + "';");
        int object_id = querymodel->data(querymodel->index(0, 0)).toInt();

        query->exec("SELECT COUNT(*) FROM teams WHERE id > 0 AND check_group(id, " + QString::number(object_id) + ");");
        querymodel->setQuery(*query);
        int count = querymodel->data(querymodel->index(0, 0)).toInt();

        int k = 1;

        m_ui->TeamsComboBox->clear();
        for (int i = 1; k <= count; ++i) {
            query->exec("SELECT name FROM teams WHERE id = " + QString::number(i) + " AND check_group(id, " + QString::number(object_id) + ");");
            querymodel->setQuery(*query);
            QString str = querymodel->data(querymodel->index(0, 0)).toString();
            if (str != "")
            {
                m_ui->TeamsComboBox->addItem(str);
                ++k;
            }
        }

        query->exec("SELECT COUNT(*) FROM labs WHERE object = " + QString::number(object_id) + ";");
        querymodel->setQuery(*query);
        count = querymodel->data(querymodel->index(0, 0)).toInt();

        k = 1;

        m_ui->NameComboBox->clear();
        for (int i = 1; k <= count; ++i) {
            query->exec("SELECT name FROM labs WHERE id = " + QString::number(i) + " AND object = " + QString::number(object_id) + ";");
            querymodel->setQuery(*query);
            QString str = querymodel->data(querymodel->index(0, 0)).toString();
            if (str != "")
            {
                m_ui->NameComboBox->addItem(str);
                ++k;
            }
        }

        query->exec("SELECT COUNT(*) FROM labs WHERE object = " + QString::number(object_id) + " AND name = '" + m_ui->NameComboBox->currentText() + "';");
        querymodel->setQuery(*query);
        count = querymodel->data(querymodel->index(0, 0)).toInt();

        k = 1;

        m_ui->VariantComboBox->clear();
        for (int i = 1; k <= count; ++i) {
            query->exec("SELECT variant FROM labs WHERE id = " + QString::number(i) + " AND object = " + QString::number(object_id) + " AND name = '" + m_ui->NameComboBox->currentText() + "';");
            querymodel->setQuery(*query);
            QString str = querymodel->data(querymodel->index(0, 0)).toString();
            if (str != "")
            {
                m_ui->VariantComboBox->addItem(str);
                ++k;
            }
        }
    });

    m_settings = new QSettings("teacher_config.ini", QSettings::IniFormat, this);
    TeacherSignIn T(m_db, m_settings);
    if (!T.exit) {
        load_TeacherInfo(m_info);

        QSqlQueryModel* querymodel = makeQuery("SELECT id, object, name, surname FROM teachers WHERE login = '" + QString(T.get_login()) + "' and password = '" + QString(T.get_password()) + "';");
        m_info.login     = T.get_login();
        m_info.password  = T.get_password();
        m_info.id        = querymodel->data(querymodel->index(0,0)).toInt();
        m_info.subjectId = querymodel->data(querymodel->index(0,1)).toInt();
        m_info.name      = querymodel->data(querymodel->index(0,2)).toString();
        m_info.surname   = querymodel->data(querymodel->index(0,3)).toString();
        QString subject;
        querymodel = makeQuery("SELECT name FROM objects WHERE objects.id = " + QString::number(m_info.id) + ";");
        subject = querymodel->data(querymodel->index(0,0)).toString();

        m_ui->infoLoginLabel->setText(m_info.login);
        m_ui->infoNameLabel->setText(m_info.name);
        m_ui->infoSurnameLable->setText(m_info.surname);
        m_ui->infoSubjectLabel->setText(subject);

        querymodel = makeQuery("select name from groups join (select group_id, objects.id as subject "
                               "from groups_and_objects join objects on groups_and_objects.object_id = objects.id) "
                               "as gr on gr.group_id = groups.id where subject = " + QString::number(m_info.subjectId) + ";");
        m_ui->allGroupsListView->setModel(querymodel);
        querymodel = makeQuery("SELECT DISTINCT name FROM objects;");
        querymodel = makeQuery("SELECT DISTINCT name FROM objects;");

        refresh();
        if (m_info.id != 1)
            m_ui->AdministrationButton->setEnabled(false);

        connect(m_ui->AdministrationButton, &QPushButton::clicked, this, &TeacherDialog::admin);

        exec();

    }
}

TeacherDialog::~TeacherDialog()
{
    delete m_ui;
}

void TeacherDialog::refresh()
{
    QSqlQuery* query = new QSqlQuery(*m_db);
    QSqlQueryModel* querymodel = new QSqlQueryModel;
    query->exec("SELECT COUNT(*) FROM objects;");
    querymodel->setQuery(*query);
    int count = querymodel->data(querymodel->index(0, 0)).toInt();

    int k = 1;

    m_ui->AddTeacherComboBox->clear();
    for (int i = 1; k <= count; ++i) {
        query->exec("SELECT name FROM objects WHERE id = " + QString::number(i) + ";");
        querymodel->setQuery(*query);
        QString str = querymodel->data(querymodel->index(0, 0)).toString();
        if (str != "")
        {
            m_ui->AddTeacherComboBox->addItem(str);
            ++k;
        }
    }

    query->exec("SELECT COUNT(*) FROM objects JOIN teachers ON objects.id = teachers.object WHERE teachers.surname = '"
                + m_info.surname + "' AND teachers.name = '" + m_info.name + "';");
    querymodel->setQuery(*query);
    count = querymodel->data(querymodel->index(0, 0)).toInt();

    k = 1;

    m_ui->CreateTaskComboBox->clear();
    for (int i = 1; k <= count; ++i)
    {
        query->exec("SELECT objects.name FROM objects JOIN teachers ON objects.id = teachers.object WHERE objects.id = "
                     + QString::number(i) + " AND teachers.surname = '"
                    + m_info.surname + "' AND teachers.name = '" + m_info.name + "';");
        querymodel->setQuery(*query);
        QString str = querymodel->data(querymodel->index(0, 0)).toString();
        if (str != "")
        {
            m_ui->CreateTaskComboBox->addItem(str);
            ++k;
        }
    }
}

void TeacherDialog::signUpTeacher() {
    m_ui->surnameErrorLabel->setText("");
    m_ui->nameErrorLabel->setText("");
    m_ui->loginErrorLabel->setText("");
    m_ui->subjectErrorLabel->setText("");
    m_ui->passwordErrorLabel->setText("");

    QSqlQueryModel* querymodel = makeQuery("SELECT login FROM teachers WHERE login = '" + m_ui->loginLine->text() + "' AND password = '" + m_ui->passwordLine->text() + "');");
    bool b = true;

    if (m_ui->passwordLine->text() != m_ui->password2Line->text()) {
        m_ui->passwordErrorLabel->setText("Пароли не совпадают!");
        b = false;
    }

    if (querymodel->rowCount() != 0) {
        m_ui->loginErrorLabel->setText("Логин или пароль уже занят!");
        b = false;
    }


    if (m_ui->surnameLine->text() == "") {
        m_ui->surnameErrorLabel->setText("Введите Вашу фамилию!");
        b = false;
    }
    if (m_ui->nameLine->text() == "") {
        m_ui->nameErrorLabel->setText("Введите Ваше имя!");
        b = false;
    }
    if (m_ui->loginLine->text() == "") {
        m_ui->loginErrorLabel->setText("Введите логин!");
        b = false;
    }
    if (m_ui->passwordLine->text() == "") {
        m_ui->passwordErrorLabel->setText("Введите пароль!");
        b = false;
    }
    if (m_ui->password2Line->text() == "") {
        m_ui->passwordErrorLabel->setText("Введите пароль повторно!");
        b = false;
    }


    querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + m_ui->AddTeacherComboBox->currentText() + "';");
    int object_id = querymodel->data(querymodel->index(0, 0)).toInt();

    querymodel = makeQuery("SELECT id FROM teachers WHERE surname = '" + m_ui->surnameLine->text()
                           + "' AND name = '" + m_ui->nameLine->text() + ";");

    if (querymodel->rowCount() != 0) {
        m_ui->surnameErrorLabel->setText("Этот пользователь уже зарегистрирован!");
        b = false;
    }

    if (b) {
        QSqlQuery* query = new QSqlQuery(*m_db);
        QSqlQueryModel* querymodel = new QSqlQueryModel;

        querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + m_ui->AddTeacherComboBox->currentText() + "';");
        int subjectId = querymodel->data(querymodel->index(0, 0)).toInt();

        query->prepare("INSERT INTO teachers VALUES (DEFAULT, :surname, :name, :object, :login, :password);");
        query->bindValue(":surname", m_ui->surnameLine->text());
        query->bindValue(":name", m_ui->nameLine->text());
        query->bindValue(":object", subjectId);
        query->bindValue(":login", m_ui->loginLine->text());
        query->bindValue(":password", m_ui->passwordLine->text());
        query->exec();
        m_ui->successLabel->setText("Пользователь успешно создан!");
    }
}

void TeacherDialog::giveTask(){
    QModelIndexList Index = m_ui->collectivesListView->selectionModel()->selectedIndexes();
    QString teamname;
    teamname = Index[0].data().toString();

    Index = m_ui->labsListView->selectionModel()->selectedIndexes();
    QString labname;
    labname = Index[0].data().toString();

    QSqlQueryModel* querymodel = makeQuery("select name from labs join (select teams.name as teamname, "
                       "teams_and_labs.laba from teams join teams_and_labs on "
                       "teams.id = teams_and_labs.team where "
                       "teams.name = '" + teamname  + "') "
                       "as tasks on tasks.laba = labs.id where name = '" + labname + "'");
    if(querymodel->rowCount() != 0){
        m_ui->errorLable->setText("Эта лабораторная уже выдана этой команде");
    } else {
        makeQuery("INSERT INTO teams_and_labs VALUES ((SELECT id FROM teams WHERE name = '" + teamname +"'),"
                  "(SELECT id FROM labs WHERE name = '" + labname + "'))");
        m_ui->errorLable->setText("Успех!");
    }
}

QSqlQueryModel* TeacherDialog::makeQuery(const QString& queryString){
    QSqlQuery* query = new QSqlQuery(*m_db);
    QSqlQueryModel *querymodel = new QSqlQueryModel;
    if (query->exec(queryString))
    {
        querymodel->setQuery(*query);
    }
    return querymodel;
}

void TeacherDialog::editProfile() {
    EditProfileDialog d;
    if (d.exec() == QDialog::Accepted) {
        TeacherInfo newInfo;
        newInfo.login = d.getNewLogin();
        newInfo.password = d.getNewPassword();

        QSqlQuery* query = new QSqlQuery(*m_db);
        QSqlQueryModel* querymodel = new QSqlQueryModel;
        if (query->exec("SELECT COUNT(*) FROM teachers WHERE login = '" + newInfo.login + "' AND password = '" + newInfo.password + "';")) {
            querymodel->setQuery(*query);
        }
        int count = querymodel->data(querymodel->index(0,0)).toInt();

        if (count != 0) {
            QMessageBox m;
            m.setInformativeText("Эти логин и пароль уже заняты!");
            m.setIcon(QMessageBox::Critical);
            m.setDefaultButton(QMessageBox::Ok);
            m.exec();
        }
        else {
            query->prepare("UPDATE teachers SET login = :newlogin, password = :newpassword WHERE login = :oldlogin AND password = :oldpassword");
            query->bindValue(":newlogin", newInfo.login);
            query->bindValue(":newpassword", newInfo.password);
            query->bindValue(":oldlogin", m_info.login);
            query->bindValue(":oldpassword", m_info.password);
            query->exec();
            m_info.login = newInfo.login;
            m_info.password = newInfo.password;
            m_ui->errorProfileLable->setText("Данные успешно изменены!");
        }
    }
}

void TeacherDialog::addTask() {
    QSqlQuery* query = new QSqlQuery(*m_db);
    QSqlQueryModel* querymodel = new QSqlQueryModel;

    querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + m_ui->CreateTaskComboBox->currentText() + "';");
    int object_id = querymodel->data(querymodel->index(0, 0)).toInt();



    querymodel = makeQuery("SELECT COUNT(*) FROM labs WHERE name = '" + m_ui->NameLineEdit->text() + "' AND object = " + QString::number(object_id) + ";");
    int variant = querymodel->data(querymodel->index(0, 0)).toInt() + 1;

    int number;
    if (variant == 1) {
        querymodel = makeQuery("SELECT COUNT(*) FROM labs WHERE object = " + QString::number(object_id) + ";");
        number = querymodel->data(querymodel->index(0, 0)).toInt();
    }
    else {
        querymodel = makeQuery("SELECT number FROM labs WHERE name = '" + m_ui->NameLineEdit->text() + "' AND object = " + QString::number(object_id) + ";");
        number = querymodel->data(querymodel->index(0, 0)).toInt();
    }


    query->exec("INSERT INTO labs VALUES (DEFAULT, '" + m_ui->NameLineEdit->text() + "', " + QString::number(object_id) + ", " + QString::number(number)
                + ", " + QString::number(variant) + ", '" + m_ui->TaskTextEdit->toPlainText() + "');");

    m_ui->CreateTaskErrorLabel->setText("Успешно!");
}

void TeacherDialog::setTask() {
    QSqlQuery* query = new QSqlQuery(*m_db);
    QSqlQueryModel* querymodel = new QSqlQueryModel;

    querymodel = makeQuery("SELECT id FROM teams WHERE name = '" + m_ui->TeamsComboBox->currentText() + "';");
    int team_id = querymodel->data(querymodel->index(0, 0)).toInt();

    querymodel = makeQuery("SELECT id FROM labs WHERE name = '" + m_ui->NameComboBox->currentText() + "' AND variant = " + m_ui->VariantComboBox->currentText() + ";");
    int laba = querymodel->data(querymodel->index(0, 0)).toInt();

    querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + m_ui->CreateTaskComboBox->currentText() + "';");
    int object_id = querymodel->data(querymodel->index(0, 0)).toInt();

    querymodel = makeQuery("SELECT * FROM teams_and_labs JOIN labs ON teams_and_labs.laba = labs.id WHERE teams_and_labs.team = " + QString::number(team_id)
                           + " AND labs.name = '" + m_ui->NameComboBox->currentText() + "' AND labs.object = " + QString::number(object_id) + ";");

    if (querymodel->rowCount() != 0)
        m_ui->CreateTaskErrorLabel->setText("Этот коллектив уже выполняет эту лабораторную работу");
    else {
        query->exec("INSERT INTO teams_and_labs VALUES (" + QString::number(team_id) + ", " + QString::number(laba) + ");");
        m_ui->CreateTaskErrorLabel->setText("Успешно!");
    }
}

void TeacherDialog::acceptTask() {
    QSqlQuery* query = new QSqlQuery(*m_db);
    QSqlQueryModel* querymodel = new QSqlQueryModel;

    querymodel = makeQuery("SELECT id FROM teams WHERE name = '" + m_ui->TeamsComboBox->currentText() + "';");
    int team_id = querymodel->data(querymodel->index(0, 0)).toInt();

    querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + m_ui->CreateTaskComboBox->currentText() + "';");
    int object_id = querymodel->data(querymodel->index(0, 0)).toInt();

    querymodel = makeQuery("SELECT laba FROM teams_and_labs JOIN labs ON teams_and_labs.laba = labs.id WHERE teams_and_labs.team = " + QString::number(team_id)
                           + " AND labs.name = '" + m_ui->NameComboBox->currentText() + "' AND labs.object = " + QString::number(object_id) + ";");

    if (querymodel->rowCount() == 0)
        m_ui->CreateTaskErrorLabel->setText("Этот коллектив не выполняет эту лабораторную работу");
    else {
        int laba = querymodel->data(querymodel->index(0, 0)).toInt();
        query->exec("DELETE FROM teams_and_labs WHERE team = " + QString::number(team_id) + " AND laba = " + QString::number(laba) + ";");
        m_ui->CreateTaskErrorLabel->setText("Успешно!");
    }
}

void TeacherDialog::exit(){
    QMessageBox m;
    m.setInformativeText("Вы действительно хотите выйти?");
    m.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    m.setIcon(QMessageBox::Warning);
    m.setDefaultButton(QMessageBox::Cancel);

    if (m.exec() == QMessageBox::Yes)
        this->close();
}

void TeacherDialog::admin() {
    Administration A(m_db);
}

void TeacherDialog::save_TeacherInfo(const TeacherInfo& info)
{
    m_settings->setValue("Login", info.login);
    m_settings->setValue("Password", info.password);
}

void TeacherDialog::load_TeacherInfo(TeacherInfo& info)
{
    info.login = m_settings->value("Login").toString();
    info.password = m_settings->value("Password").toString();
}
