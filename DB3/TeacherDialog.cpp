#include "TeacherDialog.h"
#include "ui_TeacherDialog.h"
#include <QAbstractItemModel>
#include <QErrorMessage>
#include <QDebug>

TeacherDialog::TeacherDialog(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::TeacherDialog)
{

    ui->setupUi(this);
    exec();
}

TeacherDialog::~TeacherDialog()
{
    delete ui;
}


