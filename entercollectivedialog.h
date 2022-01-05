#ifndef ENTERCOLLECTIVEDIALOG_H
#define ENTERCOLLECTIVEDIALOG_H

#include <QDialog>

namespace Ui {
class EnterCollectiveDialog;
}

class EnterCollectiveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EnterCollectiveDialog(QWidget *parent = nullptr);
    ~EnterCollectiveDialog();
    QString getName();

private:
    Ui::EnterCollectiveDialog *ui;
};

#endif // ENTERCOLLECTIVEDIALOG_H
