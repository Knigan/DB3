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
    connect(m_ui->allGroupsListView,  &QListView::clicked, [this](const QModelIndex& index){
                QSqlQueryModel* querymodel = makeQuery("SELECT DISTINCT teams.team from groups join (SELECT students.group_id as group, teams.name as team "
                                                       "FROM students JOIN teams ON students.team_id = teams.id) AS teams ON teams.group = groups.id "
                                                       "WHERE groups.name = '" + index.data().toString() + "';");
                m_ui->collectivesListView->setModel(querymodel);
    });

    connect(m_ui->collectivesListView, &QListView::clicked, [this](const QModelIndex& index){
                QSqlQueryModel* querymodel = makeQuery("select concat(students.surname, ' ', students.name) from teams join students "
                                                       "on students.team_id = teams.id where teams.name = '" + index.data().toString() + "' order by students.surname");
                m_ui->studentsListView->setModel(querymodel);
                querymodel = makeQuery("SELECT DISTINCT name FROM labs WHERE object = " + QString::number(m_info.subjectId) + ";");
                m_ui->labsListView->setModel(querymodel);
    });

    m_settings = new QSettings("connection_config.ini", QSettings::IniFormat, this);
    TeacherSignIn T(nullptr, m_settings, m_db);
    T.exec();

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
    m_ui->subjectsListView->setModel(querymodel);
    querymodel = makeQuery("SELECT DISTINCT name FROM objects;");
    m_ui->subjectsListView->setModel(querymodel);

    exec();
}

TeacherDialog::~TeacherDialog()
{
    delete m_ui;
}

void TeacherDialog::signUpTeacher(){
    m_ui->surnameErrorLabel->setText("");
    m_ui->nameErrorLabel->setText("");
    m_ui->loginErrorLabel->setText("");
    m_ui->subjectErrorLabel->setText("");
    m_ui->passwordErrorLabel->setText("");

    QSqlQueryModel* querymodel = makeQuery("SELECT login FROM teachers WHERE login = '" + m_ui->loginLine->text() + "';");
    QModelIndexList qmodellist = m_ui->subjectsListView->selectionModel()->selectedIndexes();
    if(m_ui->surnameLine->text() == "") m_ui->surnameErrorLabel->setText("Введите Вашу фамилию!");
    if(m_ui->nameLine->text() == "") m_ui->nameErrorLabel->setText("Введите Ваше имя!");
    if(m_ui->loginLine->text() == "") m_ui->loginErrorLabel->setText("Введите логин!");
    if(m_ui->passwordLine->text() == "") m_ui->passwordErrorLabel->setText("Введите пароль!");
    if(m_ui->passwordLine->text() != m_ui->password2Line->text()) m_ui->passwordErrorLabel->setText("Пароли не совпадают!");
    if(qmodellist.count() == 0) m_ui->subjectErrorLabel->setText("Выберите предмет!");
    if(querymodel->rowCount() != 0) m_ui->loginErrorLabel->setText("Логин уже занят!");
    if(querymodel->rowCount() != 0) m_ui->passwordErrorLabel->setText("Пароль уже занят!");
    if ((m_ui->surnameErrorLabel->text() == "")
            && (m_ui->nameErrorLabel->text() == "")
            && (m_ui->loginErrorLabel->text() == "")
            && (m_ui->subjectErrorLabel->text() == "")
            && (m_ui->passwordErrorLabel->text() == "")){

        int subjectId;
        QModelIndexList Index = m_ui->collectivesListView->selectionModel()->selectedIndexes();
        QString subjectname = Index[0].data().toString();
        querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + subjectname + "';");
        subjectId = querymodel->data(querymodel->index(0,0)).toInt();

        makeQuery("INSERT INTO teachers(id, surname, name, object, login, password) VALUES (DEFAULT, '"
                  + m_ui->surnameLine->text() + "', '" + m_ui->nameLine->text() + "', " + QString::number(subjectId) + ", '"
                  + m_ui->loginLine->text() + "', '" + m_ui->passwordLine->text() + "');");
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

void TeacherDialog::exit(){
    QMessageBox m;
    m.setInformativeText("Вы действительно хотите выйти?");
    m.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    m.setIcon(QMessageBox::Warning);
    m.setDefaultButton(QMessageBox::Cancel);

    if (m.exec() == QMessageBox::Yes)
        this->close();
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
