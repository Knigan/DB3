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

    connect(m_ui->editProfileButton     , &QPushButton::clicked, this, &StudentDialog::editProfile);
    connect(m_ui->exitButton            , &QPushButton::clicked, this, &StudentDialog::exit);
    connect(m_ui->refreshButton         , &QPushButton::clicked, this, &StudentDialog::refresh);
    connect(m_ui->leaveCollectiveButton , &QPushButton::clicked, this, &StudentDialog::leaveCollective);
    connect(m_ui->createCollectiveButton, &QPushButton::clicked, this, &StudentDialog::createCollective);
    connect(m_ui->enterCollectiveButton , &QPushButton::clicked, this, &StudentDialog::enterCollective);
    connect(m_ui->takeRandomTaskButton  , &QPushButton::clicked, this, &StudentDialog::takeRandomTask);

    m_settings = new QSettings("signin_config.ini", QSettings::IniFormat, this);
    SignIn S(nullptr, m_settings, m_db);
    load_StudentInfo(m_info);

    connect(m_ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
    [=](int index){
        if (index == 0)
        {
            QSqlQueryModel* querymodel = makeQuery("SELECT name FROM labs;");
            m_ui->listView->setModel(querymodel);
        }
        else
        {
            QString str;
            QSqlQueryModel* querymodel = makeQuery("SELECT name FROM labs join teams_and_labs on teams_and_labs.laba = labs.id WHERE teams_and_labs.team = " + str.setNum(m_info.teamId) + ";");
            m_ui->listView->setModel(querymodel);
        }
    });

    connect(m_ui->listView, &QListView::clicked  ,
    [this](const QModelIndex& index)
    {
        m_ui->plainTextEdit->clear();
        QSqlQueryModel* querymodel = makeQuery("SELECT task FROM labs WHERE name = '" + index.data().toString() + "';");
        m_ui->plainTextEdit->appendPlainText(querymodel->data(querymodel->index(0,0)).toString());
    });

    QSqlQueryModel* querymodel = makeQuery("SELECT surname, name, grp, team_id, id FROM students WHERE login = '" + m_info.login + "' and password = '" + m_info.password + "';");
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

void StudentDialog::acceptEntering()
{
    if (QPushButton* btn = qobject_cast<QPushButton*>(sender()))
    {
        QModelIndex index = m_ui->statementTableView->indexAt(btn->parentWidget()->pos());
        QString name = m_ui->statementTableView->model()->data(m_ui->statementTableView->model()->index(index.row(), 0)).toString();


        QSqlQuery* query = new QSqlQuery(*m_db);
        QSqlQueryModel* querymodel = makeQuery("SELECT count_for FROM requests WHERE student_id = (SELECT id FROM students WHERE name = '" + name + "');");
        int count = querymodel->data(querymodel->index(0,0)).toInt();
        query->prepare("UPDATE requests SET count_for = :count WHERE student_id = (SELECT id FROM students WHERE name = '" + name + "');");
        query->bindValue(":count", count + 1);
        query->exec();

        btn->setEnabled(false);
        btn->setText("Clicked");
        QWidget* anotherBtn = m_ui->statementTableView->indexWidget(m_ui->statementTableView->model()->index(index.row(), 3));
        anotherBtn->setEnabled(false);
    }
}

void StudentDialog::declineEntering()
{
    if (QPushButton* btn = qobject_cast<QPushButton*>(sender()))
    {
        QModelIndex index = m_ui->statementTableView->indexAt(btn->parentWidget()->pos());
        QString name = m_ui->statementTableView->model()->data(m_ui->statementTableView->model()->index(index.row(), 0)).toString();

        QSqlQuery* query = new QSqlQuery(*m_db);
        QSqlQueryModel* querymodel = makeQuery("SELECT count_against FROM requests WHERE student_id = (SELECT id FROM students WHERE name = '" + name + "');");
        int count = querymodel->data(querymodel->index(0,0)).toInt();
        query->prepare("UPDATE requests SET count_against = :count WHERE student_id = (SELECT id FROM students WHERE name = '" + name + "');");
        query->bindValue(":count", count + 1);
        query->exec();

        btn->setEnabled(false);
        btn->setText("Clicked");
        QWidget* anotherBtn = m_ui->statementTableView->indexWidget(m_ui->statementTableView->model()->index(index.row(), 2));
        anotherBtn->setEnabled(false);
    }
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
        query->prepare("UPDATE students SET team_id = 0 WHERE id = :id;");
        query->bindValue(":id", m_info.id);
        query->exec();

        refreshCount(m_info.teamId);
        m_info.teamId = 0;
        refreshCount();
        refreshCollectiveInfo();
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

void StudentDialog::refresh() {
    refreshCollectiveInfo();
    refreshRequests();
    refreshLabs();
}

void StudentDialog::createCollective()
{
    CreateCollectiveDialog d;
    if (d.exec() == QDialog::Accepted) {
        if (m_info.teamId == 0) {
            QSqlQuery* query = new QSqlQuery(*m_db);
            QString str = d.getName();
            query->prepare("INSERT INTO teams(id, name, count_of_students) VALUES (DEFAULT, :name, 1);");
            query->bindValue(":name", str);
            query->exec();

            QSqlQueryModel* querymodel = makeQuery("SELECT id FROM teams WHERE name = '" + str + "';");
            m_info.teamId = querymodel->data(querymodel->index(0,0)).toInt();
            query->prepare("UPDATE students SET team_id = :teamId WHERE id = :id;");
            query->bindValue(":teamId", m_info.teamId);
            query->bindValue(":id", m_info.id);
            query->exec();

            refreshCount();
            refreshCollectiveInfo();
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
    QSqlQueryModel* querymodel = makeQuery("SELECT name FROM teams WHERE id = " + str.setNum(m_info.teamId) + ";");
    m_ui->CollectiveName->setText(querymodel->data(querymodel->index(0,0)).toString());

    refreshCount(m_info.teamId);
    querymodel = makeQuery("SELECT count_of_students FROM teams WHERE id = " + str.setNum(m_info.teamId) + ";");
    m_ui->CollectiveNumber->setText(querymodel->data(querymodel->index(0,0)).toString());

    querymodel = makeQuery("SELECT name, surname, grp FROM students WHERE team_id = " + str.setNum(m_info.teamId) + ";");
    m_ui->CollectiveTableView->setModel(querymodel);
}

void StudentDialog::refreshRequests()
{
    QSqlQueryModel* querymodel = makeQuery("SELECT * FROM requests;");
    for (int i = 0; i < querymodel->rowCount(); ++i) {
        QString str;
        int team_id = querymodel->data(querymodel->index(i, 1)).toInt();
        int count_for = querymodel->data(querymodel->index(i, 2)).toInt();
        int count_against = querymodel->data(querymodel->index(i, 3)).toInt();

        QSqlQueryModel* querymodel2 = makeQuery("SELECT count_of_students FROM teams WHERE id = " + str.setNum(team_id) + ";");
        int count = querymodel2->data(querymodel2->index(0, 0)).toInt();

        if (count_for >= count / 2.0f) {
            int student_id = querymodel->data(querymodel->index(i, 0)).toInt();
            QSqlQuery* query = new QSqlQuery(*m_db);
            query->prepare("UPDATE students SET team_id = :team_id WHERE id = :student_id");
            query->bindValue(":team_id", team_id);
            query->bindValue(":student_id", student_id);
            query->exec();

            refreshCount();
            refreshCount(team_id);

            query->prepare("DELETE FROM requests WHERE student_id = :student_id AND team_id = :team_id");
            query->bindValue(":student_id", student_id);
            query->bindValue(":team_id", team_id);
            query->exec();
        }
        else if (count_against >= count / 2.0f) {
                int student_id = querymodel->data(querymodel->index(i, 0)).toInt();
                QSqlQuery* query = new QSqlQuery(*m_db);
                query->prepare("DELETE FROM requests WHERE student_id = :student_id AND team_id = :team_id");
                query->bindValue(":student_id", student_id);
                query->bindValue(":team_id", team_id);
                query->exec();
        }
    }

    QString str;
    querymodel = makeQuery("SELECT name, surname, grp, id FROM students WHERE id = (SELECT student_id FROM requests WHERE team_id = " + str.setNum(m_info.teamId) + ");");
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
    QSqlQueryModel* querymodel = makeQuery("SELECT name FROM labs;");
    m_ui->listView->setModel(querymodel);
}

void StudentDialog::refreshCount(int team_id) {
    QString str;
    QSqlQueryModel* querymodel = makeQuery("SELECT COUNT(*) FROM students WHERE team_id = " + str.setNum(team_id) + ";");
    int count = querymodel->data(querymodel->index(0,0)).toInt();
    QSqlQuery* query = new QSqlQuery(*m_db);
    query->prepare("UPDATE teams SET count_of_students = :count WHERE id = " + str + ";");
    query->bindValue(":count", count);
    query->exec();
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
            query->prepare("INSERT INTO requests(student_id, team_id, count_for, count_against) VALUES (:student_id, (SELECT id FROM teams WHERE name = :team_id), 0, 0);");
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
    QSqlQueryModel* querymodel = makeQuery("SELECT * FROM labs");

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
    makeQuery("insert into teams_and_labs(team, laba) VALUES (" + str.setNum(m_info.teamId) + ", " + temp.setNum(randNum) + ");");
    refreshLabs();
}

bool StudentDialog::isOk(int num)
{
    QString str;
    QSqlQueryModel* query = makeQuery("SELECT laba FROM teams_and_labs WHERE team = " + str.setNum(m_info.teamId));

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

