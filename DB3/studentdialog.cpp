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

    exec();
}


StudentDialog::~StudentDialog()
{
    delete m_ui;
}

void StudentDialog::editProfile()
{
    //qDebug() << "editProfile";
    m_studentSignIn_ui = new StudentSignIn(this);
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
