#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

public slots:
    void emailIntend();
    void httpIntend();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
