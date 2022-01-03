#ifndef ADDMEMBERDIALOG_H
#define ADDMEMBERDIALOG_H

#include <QDialog>

namespace Ui {
class AddMemberDialog;
}

class AddMemberDialog : public QDialog
{
    Q_OBJECT

public:

    AddMemberDialog(QWidget *parent = nullptr);
    ~AddMemberDialog();

private slots:

    void addMember();
    void cancel();

private:

    Ui::AddMemberDialog *ui;
};


#endif // ADDMEMBERDIALOG_H
