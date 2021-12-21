#include "StudentDialog.h"
#include "ui_studentdialog.h"

StudentDialog::StudentDialog(QDialog *parent)
    : QDialog(parent)
    , m_ui(new Ui::StudentDialog)
{
    m_ui->setupUi(this);


    connect(m_ui->editProfileButton       , &QPushButton::clicked,this, &StudentDialog::editProfile);
    connect(m_ui->addNewMemberButton      , &QPushButton::clicked,this, &StudentDialog::addNewMember);
    connect(m_ui->deleteMemberButton      , &QPushButton::clicked,this, &StudentDialog::deleteMember);
    connect(m_ui->deleteCollectiveButton  , &QPushButton::clicked,this, &StudentDialog::deleteCollective);

    m_settings = new QSettings("connection_config.ini", QSettings::IniFormat, this);
    StudentSignIn S;
    StudentInfo info;
    load_StudentInfo(info);
    S.set_surname(info.surname);
    S.set_group(info.group);

    if (S.exec() == QDialog::Accepted)
    {
        info.surname = S.get_surname();
        info.group = S.get_group();
        save_StudentInfo(info);
    }
    exec();
}


StudentDialog::~StudentDialog()
{
    delete m_ui;
}

void StudentDialog::editProfile()
{
    //qDebug() << "editProfile";
    //m_studentSignIn_ui = new StudentSignIn(this);
}

void StudentDialog::addNewMember()
{

}

void StudentDialog::deleteMember()
{

}

void StudentDialog::deleteCollective()
{

}

void StudentDialog::exit()
{

}


void StudentDialog::save_StudentInfo(const StudentInfo& info)
{
    m_settings->setValue("Surname", info.surname);
    m_settings->setValue("Group", info.group);
}

void StudentDialog::load_StudentInfo(StudentInfo& info)
{
    info.surname = m_settings->value("Surname").toString();
    info.group = m_settings->value("Group").toString();
}

//void StudentDialog::MakeQuery(const QString& a_queryString)
//{
//    if (!m_db.isOpen())
//        if(!m_db.open())
//        {
//            PrintErrors(m_db.lastError().text());
//            return;
//        }

//    QSqlQueryModel *newModel = new QSqlQueryModel;
//    newModel->setQuery(a_queryString, m_db);


//    if (newModel->query().isActive())
//    {
//        m_mainwin_ui->resultView->setModel(newModel);
//        PrintErrors("Success");
//        PrintHistory(a_queryString);
//        PrintTables();
//    }
//    else if (a_queryString.isEmpty())
//    {
//        PrintErrors("Empty query");
//    }
//    else
//    {
//       PrintErrors(newModel->query().lastError().text());
//    }
//}
