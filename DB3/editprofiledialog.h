#ifndef EDITPROFILEDIALOG_H
#define EDITPROFILEDIALOG_H

#include <QDialog>

namespace Ui {
class EditProfileDialog;
}

class EditProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditProfileDialog(QWidget *parent = nullptr);
    ~EditProfileDialog();

private slots:

    void change();
    void cancel();

private:
    Ui::EditProfileDialog *ui;
};

#endif // EDITPROFILEDIALOG_H
