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
    set_host(info.host);
    set_login(info.login);
    set_password(info.password);
    set_port(info.port);
    set_DBName(info.DBName);

    exec();
}

Connection::~Connection()
{
    delete ui;
}

QString Connection::get_host() {
    return ui->HostLineEdit->text();
}

QString Connection::get_login() {
    return ui->LoginLineEdit->text();
}

QString Connection::get_password() {
    return ui->PasswordLineEdit->text();
}

int Connection::get_port() {
    return ui->PortLineEdit->text().toInt();
}

QString Connection::get_DBName() {
    return ui->DBNameLineEdit->text();
}

void Connection::set_host(QString& host) {
    ui->HostLineEdit->insert(host);
}

void Connection::set_login(QString& login) {
    ui->LoginLineEdit->insert(login);
}

void Connection::set_password(QString& password) {
    ui->PasswordLineEdit->insert(password);
}

void Connection::set_port(int port) {
    QString str;
    ui->PortLineEdit->insert(str.setNum(port));
}

void Connection::set_DBName(QString& DBName) {
    ui->DBNameLineEdit->insert(DBName);
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
    info.host = get_host();
    info.login = get_login();
    info.password = get_password();
    info.port = get_port();
    info.DBName = get_DBName();

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
                TeacherDialog(this);
            }
            Connection();
        }
}
