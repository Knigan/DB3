#include "createcollectivedialog.h"
#include "ui_createcollectivedialog.h"

createCollectiveDialog::createCollectiveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::createCollectiveDialog)
{
    ui->setupUi(this);
    connect(ui->createNewCollectiveButton, &QPushButton::clicked,this, &createCollectiveDialog::create);
    connect(ui->cancel                   , &QPushButton::clicked,this, &createCollectiveDialog::cancel);
}

createCollectiveDialog::~createCollectiveDialog()
{
    delete ui;
}

void createCollectiveDialog::create()
{

}

void createCollectiveDialog::cancel()
{

}
