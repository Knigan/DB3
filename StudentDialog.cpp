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

    m_settings = new QSettings("student_config.ini", QSettings::IniFormat, this);
    SignIn S(m_db, m_settings);
    if (!S.exit) {
        load_StudentInfo(m_info);

        connect(m_ui->LabsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [this](){
            refreshLabs();
        });

        connect(m_ui->ObjectsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [this](){
            refreshLabs();
        });

        connect(m_ui->listView, &QListView::clicked, this,
                [this](const QModelIndex& index)
        {
            m_ui->plainTextEdit->clear();
            QSqlQueryModel* querymodel = new QSqlQueryModel;
            if (m_ui->LabsComboBox->currentIndex() == 0)
                querymodel = makeQuery("SELECT task FROM labs WHERE name = '" + index.data().toString() + "' AND variant = 1;");
            else {
                querymodel = makeQuery("SELECT variant FROM labs join teams_and_labs ON labs.id = teams_and_labs.laba WHERE teams_and_labs.team = " + QString::number(m_info.teamId)
                                       + " AND labs.name = '" + index.data().toString() + "';");
                int variant = querymodel->data(querymodel->index(0, 0)).toInt();
                querymodel = makeQuery("SELECT task FROM labs WHERE name = '" + index.data().toString() + "' AND variant = " + QString::number(variant) + ";");
            }

            m_ui->plainTextEdit->appendPlainText(querymodel->data(querymodel->index(0,0)).toString());
        });

        connect(m_ui->listView, &QListView::doubleClicked, this, [this](const QModelIndex& index) {
            if (m_info.teamId != 0) {
                srand(time(nullptr));
                QSqlQuery* query = new QSqlQuery(*m_db);
                QSqlQueryModel* querymodel = new QSqlQueryModel;

                querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + m_ui->ObjectsComboBox->currentText() + "';");
                int object_id = querymodel->data(querymodel->index(0, 0)).toInt();

                querymodel = makeQuery("SELECT id FROM labs join teams_and_labs on teams_and_labs.laba = labs.id WHERE teams_and_labs.team = "
                                       + QString::number(m_info.teamId) + " AND object = " + QString::number(object_id) + " AND name = '" + index.data().toString() + "';");
                if (querymodel->rowCount() != 0)
                    m_ui->ErrorLabel->setText("Вы уже получили это задание!");
                else {

                    querymodel = makeQuery("SELECT COUNT(*) FROM labs WHERE name = '" + index.data().toString() + "' AND object = " + QString::number(object_id) + ";");
                    int count = querymodel->data(querymodel->index(0, 0)).toInt();

                    int variant = rand() % count + 1;
                    querymodel = makeQuery("SELECT id FROM labs WHERE name = '" + index.data().toString() + "' AND object = "
                                           + QString::number(object_id) + " AND variant = " + QString::number(variant) + ";");
                    int id = querymodel->data(querymodel->index(0, 0)).toInt();

                    query->exec("INSERT INTO teams_and_labs(team, laba) VALUES (" + QString::number(m_info.teamId) + ", " + QString::number(id) + ");");

                }

            }

            refreshLabs();
        });

        QSqlQueryModel* querymodel = makeQuery("SELECT surname, name, group_id, team_id, id FROM students WHERE login = '" + m_info.login + "' and password = '" + m_info.password + "';");
        m_info.surname = querymodel->data(querymodel->index(0,0)).toString();
        m_info.name    = querymodel->data(querymodel->index(0,1)).toString();
        m_info.groupId   = querymodel->data(querymodel->index(0,2)).toInt();
        m_info.teamId  = querymodel->data(querymodel->index(0,3)).toInt();
        m_info.id = querymodel->data(querymodel->index(0,4)).toInt();


        m_ui->studentSurnameLabel->setText(m_info.surname);
        m_ui->studentNameLabel->setText(m_info.name);
        querymodel = makeQuery("SELECT name FROM groups WHERE id = " + QString::number(m_info.groupId) + ";");
        QString str = querymodel->data(querymodel->index(0, 0)).toString();
        m_ui->studentGroupLabel->setText(str);

        QSqlQuery* query = new QSqlQuery(*m_db);

        query->exec("SELECT COUNT(*) FROM objects;");
        querymodel->setQuery(*query);
        int count = querymodel->data(querymodel->index(0, 0)).toInt();

        for (int i = 1; i <= count; ++i) {
            query->exec("SELECT name FROM objects WHERE id = " + QString::number(i) + ";");
            querymodel->setQuery(*query);
            QString str = querymodel->data(querymodel->index(0, 0)).toString();
            m_ui->ObjectsComboBox->addItem(str);
        }

        refresh();
        exec();
    }
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
        int id = m_ui->statementTableView->model()->data(m_ui->statementTableView->model()->index(index.row(), 3)).toInt();


        QSqlQuery* query = new QSqlQuery(*m_db);
        QSqlQueryModel* querymodel = makeQuery("SELECT count_for FROM counts WHERE student_id = " + QString::number(id) + ";");
        int count = querymodel->data(querymodel->index(0,0)).toInt();
        query->prepare("UPDATE counts SET count_for = :count WHERE student_id = :id;");
        query->bindValue(":count", count + 1);
        query->bindValue(":id", id);
        query->exec();

        btn->setEnabled(false);
        btn->setText("Clicked");
        QWidget* anotherBtn = m_ui->statementTableView->indexWidget(m_ui->statementTableView->model()->index(index.row(), 3));
        anotherBtn->setEnabled(false);

        query->prepare("DELETE FROM requests WHERE student_id = :id AND team_id = :team_id AND student_from_team_id = :student_from_team_id;");
        query->bindValue(":id", id);
        query->bindValue(":team_id", m_info.teamId);
        query->bindValue(":student_from_team_id", m_info.id);
        query->exec();
    }
}

void StudentDialog::declineEntering()
{
    if (QPushButton* btn = qobject_cast<QPushButton*>(sender()))
    {
        QModelIndex index = m_ui->statementTableView->indexAt(btn->parentWidget()->pos());
        int id = m_ui->statementTableView->model()->data(m_ui->statementTableView->model()->index(index.row(), 3)).toInt();

        QSqlQuery* query = new QSqlQuery(*m_db);
        QSqlQueryModel* querymodel = makeQuery("SELECT count_against FROM counts WHERE student_id = " + QString::number(id) + ";");
        int count = querymodel->data(querymodel->index(0,0)).toInt();
        query->prepare("UPDATE counts SET count_against = :count WHERE student_id = :id;");
        query->bindValue(":count", count + 1);
        query->bindValue(":id", id);
        query->exec();

        btn->setEnabled(false);
        btn->setText("Clicked");
        QWidget* anotherBtn = m_ui->statementTableView->indexWidget(m_ui->statementTableView->model()->index(index.row(), 2));
        anotherBtn->setEnabled(false);

        query->prepare("DELETE FROM requests WHERE student_id = :id AND team_id = :team_id AND student_from_team_id = :student_from_team_id;");
        query->bindValue(":id", id);
        query->bindValue(":team_id", m_info.teamId);
        query->bindValue(":student_from_team_id", m_info.id);
        query->exec();
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
        QSqlQueryModel* querymodel = new QSqlQueryModel;
        if (query->exec("SELECT COUNT(*) FROM students WHERE login = '" + newInfo.login + "' AND password = '" + newInfo.password + "';")) {
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

        m_info.teamId = 0;
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
        close();
}

void StudentDialog::refresh() {
    refreshCollectiveInfo();
    refreshRequests();
    refreshLabs();
}

bool StudentDialog::isUniqueCollectiveName(const QString& name)
{
   QSqlQueryModel* querymodel = makeQuery("SELECT name FROM teams");
   for (int i = 0; i < querymodel->rowCount(); ++i)
   {
       if (name == querymodel->data(querymodel->index(i, 0)))
       {
            return false;
       }
   }
   return true;
}

void StudentDialog::createCollective()
{
    CreateCollectiveDialog d;
    if (d.exec() == QDialog::Accepted) {
        if (m_info.teamId == 0) {
            QString str = d.getName();
            if (isUniqueCollectiveName(str)) {
                QSqlQuery* query = new QSqlQuery(*m_db);
                query->prepare("INSERT INTO teams(id, name, count_of_students) VALUES (DEFAULT, :name, 1);");
                query->bindValue(":name", str);
                query->exec();

                QSqlQueryModel* querymodel = makeQuery("SELECT id FROM teams WHERE name = '" + str + "';");
                m_info.teamId = querymodel->data(querymodel->index(0,0)).toInt();
                query->prepare("UPDATE students SET team_id = :teamId WHERE id = :id;");
                query->bindValue(":teamId", m_info.teamId);
                query->bindValue(":id", m_info.id);
                query->exec();

                refreshCollectiveInfo();
            }
            else {
                QMessageBox m;
                m.setInformativeText("Это имя уже занято!");
                m.setIcon(QMessageBox::Critical);
                m.setDefaultButton(QMessageBox::Ok);
                m.exec();
            }
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
    QSqlQueryModel* querymodel = makeQuery("SELECT name FROM teams WHERE id = " + QString::number(m_info.teamId) + ";");
    m_ui->CollectiveName->setText(querymodel->data(querymodel->index(0,0)).toString());

    querymodel = makeQuery("SELECT count_of_students FROM teams WHERE id = " + QString::number(m_info.teamId) + ";");
    m_ui->CollectiveNumber->setText(querymodel->data(querymodel->index(0,0)).toString());

    querymodel = makeQuery("SELECT students.name AS Имя, surname AS Фамилия, groups.name AS Группа FROM students JOIN groups ON students.group_id = groups.id WHERE team_id = " + QString::number(m_info.teamId) + ";");
    m_ui->CollectiveTableView->setModel(querymodel);
}

void StudentDialog::refreshRequests()
{
    QSqlQueryModel* querymodel = makeQuery("SELECT * FROM counts;");
    for (int i = 0; i < querymodel->rowCount(); ++i) {
        int team_id = querymodel->data(querymodel->index(i, 1)).toInt();
        int count_for = querymodel->data(querymodel->index(i, 2)).toInt();
        int count_against = querymodel->data(querymodel->index(i, 3)).toInt();

        QSqlQueryModel* querymodel2 = makeQuery("SELECT count_of_students FROM teams WHERE id = " + QString::number(team_id) + ";");
        int count = querymodel2->data(querymodel2->index(0, 0)).toInt();

        if (count_for >= count / 2.0f) {
            int student_id = querymodel->data(querymodel->index(i, 0)).toInt();
            QSqlQuery* query = new QSqlQuery(*m_db);
            query->prepare("UPDATE students SET team_id = :team_id WHERE id = :student_id;");
            query->bindValue(":team_id", team_id);
            query->bindValue(":student_id", student_id);
            query->exec();

            query->prepare("DELETE FROM counts WHERE student_id = :student_id AND team_id = :team_id;");
            query->bindValue(":student_id", student_id);
            query->bindValue(":team_id", team_id);
            query->exec();

            query->prepare("DELETE FROM requests WHERE student_id = :student_id AND team_id = :team_id;");
            query->bindValue(":student_id", student_id);
            query->bindValue(":team_id", team_id);
            query->exec();

        }
        else if (count_against >= count / 2.0f) {
                int student_id = querymodel->data(querymodel->index(i, 0)).toInt();
                QSqlQuery* query = new QSqlQuery(*m_db);
                query->prepare("DELETE FROM counts WHERE student_id = :student_id AND team_id = :team_id;");
                query->bindValue(":student_id", student_id);
                query->bindValue(":team_id", team_id);
                query->exec();

                query->prepare("DELETE FROM requests WHERE student_id = :student_id AND team_id = :team_id;");
                query->bindValue(":student_id", student_id);
                query->bindValue(":team_id", team_id);
                query->exec();

        }
    }

    querymodel = makeQuery("SELECT COUNT(*) FROM requests WHERE student_from_team_id = " + QString::number(m_info.id));
    int count = querymodel->data(querymodel->index(0,0)).toInt();

    if (count != 0) {
        QSqlQuery* query = new QSqlQuery(*m_db);
        query->prepare("SELECT name AS Имя, surname AS Фамилия, group_id, id FROM students WHERE id = (SELECT student_id FROM requests WHERE team_id = :team_id AND student_from_team_id = :id);");
        query->bindValue(":team_id", m_info.teamId);
        query->bindValue(":id", m_info.id);
        query->exec();
        querymodel->setQuery(*query);

        querymodel->setHeaderData(2, Qt::Horizontal, "Принять");
        querymodel->setHeaderData(3, Qt::Horizontal, "Отклонить");
        m_ui->statementTableView->setModel(querymodel);

        for (int i = 0; i < querymodel->rowCount(); ++i)
        {
            m_ui->statementTableView->setIndexWidget(m_ui->statementTableView->model()->index(i,2), createAcceptButtonWidget());
            m_ui->statementTableView->setIndexWidget(m_ui->statementTableView->model()->index(i,3), createDeclineButtonWidget());
        }
    }
}

void StudentDialog::refreshLabs()
{
    QString str = m_ui->ObjectsComboBox->currentText();
    QSqlQueryModel* querymodel = makeQuery("SELECT id FROM objects WHERE name = '" + str + "';");
    int id = querymodel->data(querymodel->index(0, 0)).toInt();

    if (m_ui->LabsComboBox->currentIndex() == 0)
        querymodel = makeQuery("SELECT name FROM labs join groups_and_objects on labs.object = groups_and_objects.object_id WHERE groups_and_objects.group_id = "
                                               + QString::number(m_info.groupId) + " AND labs.object = " + QString::number(id) + " AND variant = 1;");
    else {
        if (m_info.teamId == 0)
            querymodel = makeQuery("SELECT id FROM labs WHERE 1 + 1 = 3");
        else
            querymodel = makeQuery("SELECT name FROM labs join teams_and_labs on teams_and_labs.laba = labs.id WHERE teams_and_labs.team = " + QString::number(m_info.teamId) + " AND object = " + QString::number(id) + ";");
    }

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
     EnterCollectiveDialog d(m_info.groupId, m_db);
     if (d.exec() == QDialog::Accepted)
     {
         if (m_info.teamId == 0)
         {               
                QSqlQuery* query = new QSqlQuery(*m_db);
                QString collectiveName = d.getName();
                query->prepare("INSERT INTO counts(student_id, team_id, count_for, count_against) VALUES (:student_id, (SELECT id FROM teams WHERE name = :name), 0, 0);");
                query->bindValue(":student_id", m_info.id);
                query->bindValue(":name", collectiveName);
                query->exec();


                QSqlQueryModel* querymodel = makeQuery("SELECT id FROM students WHERE team_id = (SELECT id FROM teams WHERE name = '" + collectiveName + "');");
                for (int i = 0; i < querymodel->rowCount(); ++i)
                {
                    query->prepare("INSERT INTO requests(student_id, team_id, student_from_team_id) VALUES(:student_id, (SELECT id FROM teams WHERE name = :collectiveName), :student_from_team_id);");
                    query->bindValue(":student_id", QString::number(m_info.id));
                    query->bindValue(":collectiveName", collectiveName);
                    query->bindValue(":student_from_team_id", querymodel->data(querymodel->index(i, 0)).toString());
                    query->exec();
                }
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

