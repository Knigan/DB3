#ifndef CREATECOLLECTIVEDIALOG_H
#define CREATECOLLECTIVEDIALOG_H

#include <QDialog>

namespace Ui {
class CreateCollectiveDialog;
}

class CreateCollectiveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateCollectiveDialog(QWidget *parent = nullptr);
    ~CreateCollectiveDialog();
    QString getName();

private:
    Ui::CreateCollectiveDialog *ui;
};

#endif // CREATECOLLECTIVEDIALOG_H
