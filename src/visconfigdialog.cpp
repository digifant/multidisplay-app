/*
    Copyright 2010 Stephan Martin, Dominik Gummel

    This file is part of Multidisplay.

    Multidisplay is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Multidisplay is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Multidisplay.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "visconfigdialog.h"

#include <QDebug>
#include <qwt_text.h>
#include <QPen>

VisConfigDialog::VisConfigDialog(QWidget *parent)
    : QDialog(parent) {

	ui.setupUi(this);



//	tl << "current" << "new";

//	ui.BoostPIDTableWidget->setVerticalHeaderLabels( tl );
//	for ( int r = 0 ; r <= 1 ; r++ ) {
//		for ( int c = 0 ; c <= 6 ; c++ ) {
//			QTableWidgetItem *wi = new QTableWidgetItem("");
//			Qt::ItemFlags f = Qt::NoItemFlags;
//			if ( r==1 && !(c==1) )
//				f=Qt::ItemIsEditable | Qt::ItemIsEnabled;
//			wi->setFlags(f);
//			ui.BoostPIDTableWidget->setItem(r,c, wi);
//		}
//	}

	connect ( ui.OkButton, SIGNAL(clicked()), this, SLOT(accept()) );
	connect ( ui.CancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
	connect ( ui.VisConfigTable, SIGNAL(cellChanged(int,int)), this, SLOT(cellChanged(int,int)) );
}

VisConfigDialog::~VisConfigDialog() {

}


void VisConfigDialog::readConfig (QMap<QString, QwtPlotCurve*> cfgMap) {
	ui.VisConfigTable->clear();
	ui.VisConfigTable->setColumnCount(4);
	ui.VisConfigTable->setRowCount(cfgMap.count() );

	QList<QString> tl;
	tl << "MD name" << "UI name" << "color" << "show" ;
	ui.VisConfigTable->setHorizontalHeaderLabels( tl );

	int i = 0;
	foreach (QString s, cfgMap.keys()) {
		QTableWidgetItem *wi = new QTableWidgetItem(s);
		//md name
		Qt::ItemFlags f = Qt::NoItemFlags;
		wi->setFlags(f);
		ui.VisConfigTable->setItem (i,0,wi);

		wi = new QTableWidgetItem();
		f=Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
		QwtPlotCurve* pc = cfgMap[s];
		if (pc) {
			//visible
			if ( cfgMap[s]->isVisible() )
				wi->setCheckState(Qt::Checked);
			else
				wi->setCheckState(Qt::Unchecked);
			wi->setFlags(f);
			ui.VisConfigTable->setItem (i,3,wi);

			//ui name
			wi = new QTableWidgetItem( pc->title().text() );
			ui.VisConfigTable->setItem (i,1,wi);

			//color
			wi = new QTableWidgetItem( pc->pen().color().name() );
			wi->setBackground ( pc->pen().brush() );
			ui.VisConfigTable->setItem (i,2,wi);
//			QComboBox* c = new QComboBox(NULL);
//			ui.VisConfigTable->setCellWidget(i,2, c );

//			wi = new QTableWidgetItem( pc->title().text() );
//			ui.VisConfigTable->setItem (i,1,wi);
		}

		i++;
	}
}


QTableWidget* VisConfigDialog::getCfgTableWidget () {
	return ui.VisConfigTable;
}

void VisConfigDialog::cellChanged (int row, int column) {
	if ( column == 2 ) {
		qDebug() << "color cell in row " << row << " changed";
		if ( ui.VisConfigTable->item(row,column) ) {
			QString cs = ui.VisConfigTable->item(row,column)->text();
			QColor color (cs);
			if ( color.isValid() )
				ui.VisConfigTable->item(row,column)->setBackground ( QBrush(color) );
		}
	}
}
