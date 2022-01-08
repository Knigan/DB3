#include "connection.h"
#include "ui_connection.h"

Connection::Connection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connection) {
    ui->setupUi(this);
    connection_settings = new QSettings("connection_config.ini", QSettings::IniFormat, this);
    connect(ui->AcceptButton, &QPushButton::clicked, this, &Connection::accept);
    ui->RoleBox->addItem("Студент");
    ui->RoleBox->addItem("Преподаватель");

    ConnectionInfo info;
    load_ConnectionInfo(info);
    ui->HostLineEdit->insert(info.host);
    ui->LoginLineEdit->insert(info.login);
    ui->PasswordLineEdit->insert(info.password);
    ui->PortLineEdit->insert(QString::number(info.port));
    ui->DBNameLineEdit->insert(info.DBName);

    exec();
}

Connection::~Connection()
{
    delete ui;
}

void Connection::save_ConnectionInfo(const ConnectionInfo &info) {
    connection_settings->setValue("Host", info.host);
    connection_settings->setValue("Port", info.port);
    connection_settings->setValue("DatabaseName", info.DBName);
    connection_settings->setValue("Login", info.login);
    connection_settings->setValue("Password", info.password);
}

void Connection::load_ConnectionInfo(ConnectionInfo& info) {
    info.host = connection_settings->value("Host").toString();
    info.port = connection_settings->value("Port").toInt();
    info.DBName = connection_settings->value("DatabaseName").toString();
    info.login = connection_settings->value("Login").toString();
    info.password = connection_settings->value("Password").toString();
}

void Connection::accept() {
    DB = QSqlDatabase::addDatabase("QPSQL", "Database");

    ConnectionInfo info;
    info.host = ui->HostLineEdit->text();
    info.login = ui->LoginLineEdit->text();
    info.password = ui->PasswordLineEdit->text();
    info.port = ui->PortLineEdit->text().toInt();
    info.DBName = ui->DBNameLineEdit->text();

    DB.setHostName(info.host);
    DB.setPort(info.port);
    DB.setDatabaseName(info.DBName);
    DB.setUserName(info.login);
    DB.setPassword(info.password);

    if (!DB.open()) {
           ui->ErrorLabel->setText("Database connection has failed!");
        } else {
            ui->ErrorLabel->setText("Database connection has been finished successfully!");
            save_ConnectionInfo(info);
            hide();
            if (ui->RoleBox->itemText(ui->RoleBox->currentIndex()) == "Студент") {
                StudentDialog(this, &DB);
            }
            else {
                TeacherDialog(this, &DB);
            }
            Connection();
        }
}
