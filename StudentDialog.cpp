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
    connect(m_ui->deleteCollectiveButton, &QPushButton::clicked,this, &StudentDialog::deleteCollective);
    connect(m_ui->createCollectiveButton, &QPushButton::clicked,this, &StudentDialog::createCollective);
    connect(m_ui->enterCollectiveButton , &QPushButton::clicked,this, &StudentDialog::enterCollective);
    connect(m_ui->takeTaskButton        , &QPushButton::clicked,this, &StudentDialog::takeTask);
    connect(m_ui->takeRandomTaskButton  , &QPushButton::clicked,this, &StudentDialog::takeRandomTask);

    m_settings = new QSettings("signin_config.ini", QSettings::IniFormat, this);
    SignIn S;

    load_StudentInfo(m_info);
    S.set_login(m_info.login);
    S.set_password(m_info.password);

    if (S.exec() == QDialog::Accepted)
    {
        m_info.login = S.get_login();
        m_info.password = S.get_password();
        save_StudentInfo(m_info);
        QSqlQueryModel *querymodel = makeQuery("select surname, name, grp, team_id, id from students where login = '" + m_info.login + "' and password = '" + m_info.password + "';");
        m_info.surname = querymodel->data(querymodel->index(0,0)).toString();
        m_info.name    = querymodel->data(querymodel->index(0,1)).toString();
        m_info.group   = querymodel->data(querymodel->index(0,2)).toString();
        m_info.teamId  = querymodel->data(querymodel->index(0,3)).toInt();
        m_info.id = querymodel->data(querymodel->index(0,4)).toInt();


        m_ui->studentSurnameLabel->setText(m_info.surname);
        m_ui->studentNameLabel->setText(m_info.name);
        m_ui->studentGroupLabel->setText(m_info.group);

        QString str;
        querymodel = makeQuery("select name from teams where id = " + str.setNum(m_info.teamId) + ";");
        m_ui->CollectiveName->setText(querymodel->data(querymodel->index(0,0)).toString());

        querymodel = makeQuery("select surname, name, grp, team_id from students where login = '" + m_info.login + "' and password = '" + m_info.password + "';");
        querymodel = makeQuery("select name, surname, grp from students where team_id = " + str.setNum(m_info.teamId) + ";");
        m_ui->CollectiveTableView->setModel(querymodel);

        exec();
    }
}



void StudentDialog::acceptEntering()
{

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
        QSqlQuery query(*m_db);
        query.prepare("UPDATE students SET login = :newlogin, password = :newpassword WHERE login = :oldlogin AND password = :oldpassword");
        query.bindValue(":newlogin", newInfo.login);
        query.bindValue(":newpassword", newInfo.password);
        query.bindValue(":oldlogin", m_info.login);
        query.bindValue(":oldpassword", m_info.password);
        query.exec();
        m_info.login = newInfo.login;
        m_info.password = newInfo.password;
        save_StudentInfo(m_info);
    }
}

void StudentDialog::addNewMember()
{
    //AddMemberDialog d;
    //d.exec();
}

void StudentDialog::deleteMember()
{
    //deleteMemberDialog d(this);
    //d.exec();
}

void StudentDialog::deleteCollective()
{
    QMessageBox m;
    m.setInformativeText("Вы действительно хотите распустить коллектив?");
    m.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    m.setIcon(QMessageBox::Critical);
    m.setDefaultButton(QMessageBox::Cancel);

    int res = m.exec();
    if (res == QMessageBox::Yes)
    {
        m_ui->CollectiveName->setText("Вы не состоите в коллективе");
    }

}

void StudentDialog::exit()
{
    QMessageBox m;
    m.setInformativeText("Вы действительно хотите выйти?");
    m.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    m.setIcon(QMessageBox::Warning);
    m.setDefaultButton(QMessageBox::Cancel);

    int res = m.exec();
    if (res == QMessageBox::Yes)
        this->close();
}

void StudentDialog::createCollective()
{
    CreateCollectiveDialog d;
    if (d.exec() == QDialog::Accepted) {
        if (m_info.teamId == 0) {
            QSqlQuery query;
            query.prepare("INSERT INTO teams(id, name, count_of_students) VALUES (DEFAULT, :name, 1);");
            query.bindValue(":name", d.getName());
            query.exec();
            QSqlQueryModel* querymodel = makeQuery("select id from teams where name = '" + d.getName() + "';");
            m_info.teamId = querymodel->data(querymodel->index(0,0)).toInt();
            query.prepare("UPDATE students SET team_id = :teamId WHERE id = ':id';");
            query.bindValue(":teamId", m_info.teamId);
            query.bindValue(":id", m_info.id);
            query.exec();
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
     /*EnterCollectiveDialog d;
     if (d.exec() == QDialog::Accepted)
     {
         qDebug() << "loh";
     }*/
}

void StudentDialog::takeTask()
{

}

void StudentDialog::takeRandomTask()
{

}


void StudentDialog::save_StudentInfo(const StudentInfo& info)
{
    m_settings->setValue("Login", info.login);
    m_settings->setValue("Password"  , info.password);
}

void StudentDialog::load_StudentInfo(StudentInfo& info)
{
    info.login = m_settings->value("Login").toString();
    info.password = m_settings->value("Password").toString();
}
