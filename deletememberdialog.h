#ifndef DELETEMEMBERDIALOG_H
#define DELETEMEMBERDIALOG_H

#include <QDialog>

namespace Ui {
class deleteMemberDialog;
}

class deleteMemberDialog : public QDialog
{
    Q_OBJECT

public:
    explicit deleteMemberDialog(QWidget *parent = nullptr);
    ~deleteMemberDialog();

private slots:
    void deleteMember();

private:
    Ui::deleteMemberDialog *ui;
};

#endif // DELETEMEMBERDIALOG_H
