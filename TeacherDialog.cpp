#include "TeacherDialog.h"
#include "teachersignin.h"
#include "TeacherDialog.h"
#include "ui_TeacherDialog.h"

#include <QAbstractItemModel>
#include <QErrorMessage>
#include <QDebug>

TeacherDialog::TeacherDialog(QDialog *parent, QSqlDatabase* p) :
    QDialog(parent),
    m_db(p),
    m_ui(new Ui::TeacherDialog)
{
    m_ui->setupUi(this);

    //m_ui->subjectsListView->QAbstractItemView::MultiSelection->;

    connect(m_ui->exitButton,         &QPushButton::clicked, this, &TeacherDialog::exit);
    connect(m_ui->editProfileButton,  &QPushButton::clicked, this, &TeacherDialog::editProfile);
    connect(m_ui->allGroupsListView,     &QListView::clicked, [this](const QModelIndex& index){
        QSqlQueryModel* querymodel = makeQuery("select distinct teams.name from teams join students on students.team_id"
                                               " = teams.id where students.grp = '" + index.data().toString() + "'");
        m_ui->collectivesListView->setModel(querymodel);
    });
    connect(m_ui->collectivesListView, &QListView::clicked, [this](const QModelIndex& index){
        QSqlQueryModel* querymodel = makeQuery("select concat(students.surname, ' ', students.name) from teams join students "
                                               "on students.team_id = teams.id where teams.name = '" + index.data().toString() + "' order by students.surname");
        m_ui->studentsListView->setModel(querymodel);
        querymodel = makeQuery("select labs.name from labs join (select teams.name as teamname, teams_and_labs.laba as labnum "
                               "from teams join teams_and_labs on teams.id = teams_and_labs.team) as alala on labnum = labs.id "
                               "where teamname = '" + index.data().toString() + "'");
        m_ui->labsListView->setModel(querymodel);
    });

    m_settings = new QSettings("connection_config.ini", QSettings::IniFormat, this);
    TeacherSignIn T;

    QSqlQueryModel* querymodel = makeQuery("select DISTINCT grp from students ;");
    m_ui->allGroupsListView->setModel(querymodel);

    if(T.exec() == QDialog::Accepted){

    }

    exec();
}

TeacherDialog::~TeacherDialog()
{
    delete m_ui;
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
    if (d.exec() == QDialog::Accepted){
        TeacherInfo newInfo;
        newInfo.login = d.getNewLogin();
        newInfo.password = d.getNewPassword();
        //query->prepare("UPDATE students SET login = :newlogin, password = :newpassword WHERE login = :oldlogin AND password = :oldpassword");
        //query->bindValue(":newlogin", newInfo.login);
        //query->bindValue(":newpassword", newInfo.password);
        //query->bindValue(":oldlogin", m_info.login);
        //query->bindValue(":oldpassword", m_info.password);
        //query->exec();
        m_info.login = newInfo.login;
        m_info.password = newInfo.password;
        save_TeacherInfo(m_info);
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

void TeacherDialog::save_TeacherInfo(const TeacherInfo& info){
    m_settings->setValue("Login", info.login);
    m_settings->setValue("Password", info.password);
}
