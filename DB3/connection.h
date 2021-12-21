#ifndef CONNECTION_H
#define CONNECTION_H

#include "StudentDialog.h"
#include "TeacherDialog.h"

#include <QDialog>
#include <QSqlDatabase>
#include <QSettings>

namespace Ui {
class Connection;
}

struct ConnectionInfo {
    QString host;
    QString login;
    QString password;
    int port;
    QString DBName;
};

class Connection : public QDialog
{
    Q_OBJECT

public:

    explicit Connection(QWidget *parent = nullptr);
    ~Connection();
    QString get_host();
    QString get_login();
    QString get_password();
    int get_port();
    QString get_DBName();
    void set_host(QString&);
    void set_login(QString&);
    void set_password(QString&);
    void set_port(int);
    void set_DBName(QString&);


private slots:

    void accept();

private:
    Ui::Connection *ui;
    QSettings* connection_settings;
    QSqlDatabase DB;
    void save_ConnectionInfo(const ConnectionInfo&);
    void load_ConnectionInfo(ConnectionInfo&);
};

#endif // CONNECTION_H
