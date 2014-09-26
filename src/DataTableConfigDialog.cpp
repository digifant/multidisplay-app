#include "DataTableConfigDialog.h"
#include "ui_DataTableConfigDialog.h"

#include <QDebug>

DataTableConfigDialog::DataTableConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataTableConfigDialog)
{
    ui->setupUi(this);

}

DataTableConfigDialog::~DataTableConfigDialog()
{
    delete ui;
}

void DataTableConfigDialog::accept() {
    QDialog::accept();
}

void DataTableConfigDialog::readConfig (QMap<QString,bool> &visMap, QVector<QString> &names) {
    ui->DataTableConfigTable->clear();
    ui->DataTableConfigTable->setColumnCount(2);
    ui->DataTableConfigTable->setRowCount(names.count() );

    QList<QString> tl;
    tl << "name" << "show";
//    tl << "Name" << "show" << "color" << "warn color 1" << "warn color 2" << "val" << "warn val 1" << "warn val 2";
    ui->DataTableConfigTable->setHorizontalHeaderLabels( tl );

    for (quint8 c = 0 ; c < names.size() ; c++ ) {
        QTableWidgetItem *wi = new QTableWidgetItem(names.at(c));
        //md name
        Qt::ItemFlags f = Qt::NoItemFlags;
        wi->setFlags(f);
        ui->DataTableConfigTable->setItem (c,0,wi);

        wi = new QTableWidgetItem();
        f=Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
        //visible
        if ( visMap[names[c]] == true )
            wi->setCheckState(Qt::Checked);
        else
            wi->setCheckState(Qt::Unchecked);
        wi->setFlags(f);
        ui->DataTableConfigTable->setItem (c,1,wi);

        //color
//		wi = new QTableWidgetItem( pc->pen().color().name() );
//		wi->setBackground ( pc->pen().brush() );
//		ui->VisConfigTable->setItem (i,2,wi);
    }
}

QTableWidget* DataTableConfigDialog::getCfgTableWidget () {
    return ui->DataTableConfigTable;
}
