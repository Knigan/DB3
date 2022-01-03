#ifndef CREATECOLLECTIVEDIALOG_H
#define CREATECOLLECTIVEDIALOG_H

#include <QDialog>

namespace Ui {
class createCollectiveDialog;
}

class createCollectiveDialog : public QDialog
{
    Q_OBJECT

public:

    explicit createCollectiveDialog(QWidget *parent = nullptr);
    ~createCollectiveDialog();

private slots:

    void create();
    void cancel();

private:
    Ui::createCollectiveDialog *ui;
};

#endif // CREATECOLLECTIVEDIALOG_H
