#include "TeacherDialog.h"
#include "teachersignin.h"
#include "ui_TeacherDialog.h"

#include <QAbstractItemModel>
#include <QErrorMessage>
#include <QDebug>

TeacherDialog::TeacherDialog(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::TeacherDialog)
{
    ui->setupUi(this);
    m_settings = new QSettings("connection_config.ini", QSettings::IniFormat, this);
    TeacherSignIn T;

    T.exec();

    exec();
}

TeacherDialog::~TeacherDialog()
{
    delete ui;
}


