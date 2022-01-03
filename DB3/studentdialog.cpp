#include "StudentDialog.h"
#include "ui_studentdialog.h"

StudentDialog::StudentDialog(QDialog *parent)
    : QDialog(parent)
    , m_ui(new Ui::StudentDialog)
{
    m_ui->setupUi(this);

    connect(m_ui->editProfileButton     , &QPushButton::clicked,this, &StudentDialog::editProfile);
    connect(m_ui->exitButton            , &QPushButton::clicked,this, &StudentDialog::exit);
    connect(m_ui->addNewMemberButton    , &QPushButton::clicked,this, &StudentDialog::addNewMember);
    connect(m_ui->deleteMemberButton    , &QPushButton::clicked,this, &StudentDialog::deleteMember);
    connect(m_ui->deleteCollectiveButton, &QPushButton::clicked,this, &StudentDialog::deleteCollective);
    connect(m_ui->createCollectiveButton, &QPushButton::clicked,this, &StudentDialog::createCollective);
    connect(m_ui->enterCollectiveButton , &QPushButton::clicked,this, &StudentDialog::enterCollective);
    connect(m_ui->takeTaskButton        , &QPushButton::clicked,this, &StudentDialog::takeTask);
    connect(m_ui->takeRandomTaskButton  , &QPushButton::clicked,this, &StudentDialog::takeRandomTask);

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

    m_ui->studentNameLabel->setText(info.surname);
    m_ui->studentGroupLabel->setText(info.group);
    m_ui->CollectiveName->setText("Коллектив");
    exec();
}


StudentDialog::~StudentDialog()
{
    delete m_ui;
}

void StudentDialog::editProfile()
{
    EditProfileDialog d(this);
    d.exec();
}

void StudentDialog::addNewMember()
{
    AddMemberDialog d;
    d.exec();
}

void StudentDialog::deleteMember()
{
    deleteMemberDialog d(this);
    d.exec();
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
    {

    }
}

void StudentDialog::createCollective()
{
    createCollectiveDialog d;
    d.exec();
}

void StudentDialog::enterCollective()
{
     EnterCollectiveDialog d;
     d.exec();
}

void StudentDialog::takeTask()
{

}

void StudentDialog::takeRandomTask()
{

}


void StudentDialog::save_StudentInfo(const StudentInfo& info)
{
    m_settings->setValue("Surname", info.surname);
    m_settings->setValue("Group"  , info.group);
}

void StudentDialog::load_StudentInfo(StudentInfo& info)
{
    info.surname = m_settings->value("Surname").toString();
    info.group   = m_settings->value("Group").toString();
}
