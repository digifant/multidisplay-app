#ifndef DATATABLECONFIGDIALOG_H
#define DATATABLECONFIGDIALOG_H

#include <QDialog>

class QTableWidget;

namespace Ui {
class DataTableConfigDialog;
}

class DataTableConfigDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DataTableConfigDialog(QWidget *parent = 0);
    ~DataTableConfigDialog();
    
    void readConfig (QMap<QString,bool> &visMap, QVector<QString> &names);
    QTableWidget* getCfgTableWidget ();

//public slots:
//    void cellChanged (int row, int column);
    void accept();

private:
    Ui::DataTableConfigDialog *ui;
};

#endif // DATATABLECONFIGDIALOG_H
