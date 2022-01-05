#include "StudentDialog.h"
#include "ui_studentdialog.h"
#include <QDialogButtonBox>
#include <QSqlTableModel>

StudentDialog::StudentDialog(QDialog *parent, QSqlDatabase* p)
    : QDialog(parent)
    , m_db(p)
    , m_ui(new Ui::StudentDialog)
{
    m_ui->setupUi(this);

    connect(m_ui->editProfileButton     , &QPushButton::clicked,this, &StudentDialog::editProfile);
    connect(m_ui->exitButton            , &QPushButton::clicked,this, &StudentDialog::exit);
    connect(m_ui->leaveCollectiveButton, &QPushButton::clicked,this, &StudentDialog::leaveCollective);
    connect(m_ui->createCollectiveButton, &QPushButton::clicked,this, &StudentDialog::createCollective);
    connect(m_ui->enterCollectiveButton , &QPushButton::clicked,this, &StudentDialog::enterCollective);
    connect(m_ui->takeRandomTaskButton  , &QPushButton::clicked,this, &StudentDialog::takeRandomTask);

    m_settings = new QSettings("signin_config.ini", QSettings::IniFormat, this);
    SignIn S(nullptr, m_settings, m_db);
    load_StudentInfo(m_info);

    connect(m_ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
    [=](int index){
        if (index == 0)
        {
            QSqlQueryModel* querymodel = makeQuery("select name from labs;");
            m_ui->listView->setModel(querymodel);
        }
        else
        {
            QString str;
            QSqlQueryModel* querymodel = makeQuery("select name from labs join teams_and_labs on teams_and_labs.laba = labs.id where teams_and_labs.team = " + str.setNum(m_info.teamId) + ";");
            m_ui->listView->setModel(querymodel);
        }
    });

    connect(m_ui->listView, &QListView::clicked  ,
    [this](const QModelIndex& index)
    {
        m_ui->plainTextEdit->clear();
        QSqlQueryModel* querymodel = makeQuery("select task from labs where name = '" + index.data().toString() + "';");
        m_ui->plainTextEdit->appendPlainText(querymodel->data(querymodel->index(0,0)).toString());
    });

    QSqlQueryModel *querymodel = makeQuery("select surname, name, grp, team_id, id from students where login = '" + m_info.login + "' and password = '" + m_info.password + "';");
    m_info.surname = querymodel->data(querymodel->index(0,0)).toString();
    m_info.name    = querymodel->data(querymodel->index(0,1)).toString();
    m_info.group   = querymodel->data(querymodel->index(0,2)).toString();
    m_info.teamId  = querymodel->data(querymodel->index(0,3)).toInt();
    m_info.id = querymodel->data(querymodel->index(0,4)).toInt();


    m_ui->studentSurnameLabel->setText(m_info.surname);
    m_ui->studentNameLabel->setText(m_info.name);
    m_ui->studentGroupLabel->setText(m_info.group);

    refreshCollectiveInfo();
    refreshRequests();
    refreshLabs();

    exec();
}

QWidget* StudentDialog::createAcceptButtonWidget()
{
    QPushButton* b = new QPushButton("Accept");
    connect(b, &QPushButton::clicked, this, &StudentDialog::acceptEntering);
    return b;
}

QWidget* StudentDialog::createDeclineButtonWidget()
{
    QPushButton* b = new QPushButton("Decline");
    connect(b, &QPushButton::clicked, this, &StudentDialog::declineEntering);
    return b;
}

void StudentDialog::acceptEntering() {
    QSqlQuery* query = new QSqlQuery(*m_db);
    QSqlQueryModel* querymodel = makeQuery("select count_for from requests where id = 0;");
    int count = querymodel->data(querymodel->index(0,0)).toInt();
    query->prepare("UPDATE requests SET count_for = :count WHERE id = 0;");
    query->bindValue(":count", count + 1);
    query->exec();
}

void StudentDialog::declineEntering()
{
    QSqlQuery* query = new QSqlQuery(*m_db);
    QSqlQueryModel* querymodel = makeQuery("select count_against from requests where id = 0;");
    int count = querymodel->data(querymodel->index(0,0)).toInt();
    query->prepare("UPDATE requests SET count_against = :count WHERE id = 0;");
    query->bindValue(":count", count + 1);
    query->exec();
}

StudentDialog::~StudentDialog()
{
    delete m_ui;
}

void StudentDialog::editProfile()
{
    EditProfileDialog d;
    if (d.exec() == QDialog::Accepted) {
        StudentInfo newInfo;
        newInfo.login = d.getNewLogin();
        newInfo.password = d.getNewPassword();
        QSqlQuery* query = new QSqlQuery(*m_db);
        query->prepare("UPDATE students SET login = :newlogin, password = :newpassword WHERE login = :oldlogin AND password = :oldpassword");
        query->bindValue(":newlogin", newInfo.login);
        query->bindValue(":newpassword", newInfo.password);
        query->bindValue(":oldlogin", m_info.login);
        query->bindValue(":oldpassword", m_info.password);
        query->exec();
        m_info.login = newInfo.login;
        m_info.password = newInfo.password;
        save_StudentInfo(m_info);
    }
}

void StudentDialog::leaveCollective()
{
    QMessageBox m;
    m.setInformativeText("Вы действительно хотите выйти из коллектива?");
    m.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    m.setIcon(QMessageBox::Warning);
    m.setDefaultButton(QMessageBox::Cancel);

    if (m.exec() == QMessageBox::Yes)
    {
        QSqlQuery* query = new QSqlQuery(*m_db);
        QString str;
        QSqlQueryModel* querymodel = makeQuery("select count_of_students from teams where id = " + str.setNum(m_info.teamId) + ";");
        int count = querymodel->data(querymodel->index(0,0)).toInt();
        query->prepare("UPDATE teams SET count_of_students = :count WHERE id = :id;");
        query->bindValue(":count", count - 1);
        query->bindValue(":id", m_info.teamId);
        query->exec();

        query->prepare("UPDATE students SET team_id = 0 WHERE id = :id;");
        query->bindValue(":id", m_info.id);
        query->exec();
        m_info.teamId = 0;

        querymodel = makeQuery("select count_of_students from teams where id = 0;");
        count = querymodel->data(querymodel->index(0,0)).toInt();
        query->prepare("UPDATE teams SET count_of_students = :count WHERE id = 0;");
        query->bindValue(":count", count + 1);
        query->exec();

        refreshCollectiveInfo();
        refreshRequests();
        refreshLabs();
    }
}

void StudentDialog::exit()
{
    QMessageBox m;
    m.setInformativeText("Вы действительно хотите выйти?");
    m.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    m.setIcon(QMessageBox::Warning);
    m.setDefaultButton(QMessageBox::Cancel);

    if (m.exec() == QMessageBox::Yes)
        this->close();
}

void StudentDialog::createCollective()
{
    CreateCollectiveDialog d;
    if (d.exec() == QDialog::Accepted) {
        if (m_info.teamId == 0) {
            QSqlQuery* query = new QSqlQuery(*m_db);

            QSqlQueryModel* querymodel = makeQuery("select count_of_students from teams where id = 0;");
            int count = querymodel->data(querymodel->index(0,0)).toInt();
            query->prepare("UPDATE teams SET count_of_students = :count WHERE id = 0;");
            query->bindValue(":count", count - 1);
            query->exec();

            QString str = d.getName();
            query->prepare("INSERT INTO teams(id, name, count_of_students) VALUES (DEFAULT, :name, 1);");
            query->bindValue(":name", str);
            query->exec();

            querymodel = makeQuery("select id from teams where name = '" + str + "';");
            m_info.teamId = querymodel->data(querymodel->index(0,0)).toInt();
            query->prepare("UPDATE students SET team_id = :teamId WHERE id = :id;");
            query->bindValue(":teamId", m_info.teamId);
            query->bindValue(":id", m_info.id);
            query->exec();

            refreshCollectiveInfo();
            refreshRequests();
            refreshLabs();
        }
        else {
            QMessageBox m;
            m.setInformativeText("Вы уже состоите в коллективе!");
            m.setIcon(QMessageBox::Critical);
            m.setDefaultButton(QMessageBox::Ok);
            m.exec();
        }
    }
}

void StudentDialog::refreshCollectiveInfo()
{
    QString str;
    QSqlQueryModel* querymodel = makeQuery("select name from teams where id = " + str.setNum(m_info.teamId) + ";");
    m_ui->CollectiveName->setText(querymodel->data(querymodel->index(0,0)).toString());

    querymodel = makeQuery("select count_of_students from teams where id = " + str.setNum(m_info.teamId) + ";");
    m_ui->CollectiveNumber->setText(querymodel->data(querymodel->index(0,0)).toString());

    querymodel = makeQuery("select name, surname, grp from students where team_id = " + str.setNum(m_info.teamId) + ";");
    m_ui->CollectiveTableView->setModel(querymodel);
}

void StudentDialog::refreshRequests()
{
    QString str;
    QSqlQueryModel* querymodel = makeQuery("select name, surname, grp, id from students where id = (select student_id from requests where team_id = " + str.setNum(m_info.teamId) + ");");
    querymodel->setHeaderData(2, Qt::Horizontal, "Принять");
    querymodel->setHeaderData(3, Qt::Horizontal, "Отклонить");
    m_ui->statementTableView->setModel(querymodel);

    for (int i = 0; i < querymodel->rowCount(); ++i)
    {
        m_ui->statementTableView->setIndexWidget(m_ui->statementTableView->model()->index(i,2), createAcceptButtonWidget());
        m_ui->statementTableView->setIndexWidget(m_ui->statementTableView->model()->index(i,3), createDeclineButtonWidget());
    }
}

void StudentDialog::refreshLabs()
{
    QSqlQueryModel* querymodel = makeQuery("select name from labs;");
    m_ui->listView->setModel(querymodel);
}


QSqlQueryModel* StudentDialog::makeQuery(const QString& queryString)
{
    QSqlQuery* query = new QSqlQuery(*m_db);
    QSqlQueryModel *querymodel = new QSqlQueryModel;
    if (query->exec(queryString))
    {
        querymodel->setQuery(*query);
    }
    return querymodel;
}

void StudentDialog::enterCollective()
{
     EnterCollectiveDialog d;
     if (d.exec() == QDialog::Accepted)
     {
         if (m_info.teamId == 0)
         {
            QSqlQuery* query = new QSqlQuery(*m_db);
            query->prepare("INSERT INTO requests(student_id, team_id, count_for, count_against) VALUES (:student_id, :team_id, 0, 0);");
            query->bindValue(":student_id", m_info.id);
            query->bindValue(":team_id", d.getName());
            query->exec();
         }
         else
         {
             QMessageBox m;
             m.setInformativeText("Вы уже состоите в коллективе!");
             m.setIcon(QMessageBox::Critical);
             m.setDefaultButton(QMessageBox::Ok);
             m.exec();
         }
     }
}

void StudentDialog::takeRandomTask()
{
    srand(time(nullptr));
    QString str, temp;
    QSqlQueryModel* querymodel = makeQuery("select * from labs");

    int randNum = rand() % querymodel->rowCount();
    qDebug() << randNum << " first" ;
    bool flag = true;

    while (flag)
    {
        if (isOk(randNum))
        {
            flag = false;
        }
        else
        {
            randNum = rand() % querymodel->rowCount();
        }
    }
    qDebug() << randNum << " - result";
    makeQuery("insert into teams_and_labs(team, laba) values (" + str.setNum(m_info.teamId) + ", " + temp.setNum(randNum) + ");");
    refreshLabs();
}

bool StudentDialog::isOk(int num)
{
    QString str;
    QSqlQueryModel* query = makeQuery("select laba from teams_and_labs where team = " + str.setNum(m_info.teamId));

    for(int i = 0; i < query->rowCount(); ++i)
    {
        if (num == query->data(query->index(i, 0)).toInt() || num == 0)
        {
            return false;
        }
    }
    return true;
}


void StudentDialog::save_StudentInfo(const StudentInfo& info)
{
    m_settings->setValue("Login", info.login);
    m_settings->setValue("Password", info.password);
}

void StudentDialog::load_StudentInfo(StudentInfo& info)
{
    info.login = m_settings->value("Login").toString();
    info.password = m_settings->value("Password").toString();
}

