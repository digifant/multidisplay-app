/*
    Copyright 2012 Dominik Gummel

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

#include "MyTableWidget.h"
#include <QKeyEvent>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>

MyTableWidget::MyTableWidget(QWidget *parent) :
    QTableWidget(parent)
{
}

void MyTableWidget::keyPressEvent(QKeyEvent *event){
    bool handled = false;
    if(event->type() == QKeyEvent::KeyPress) {
        if( event->matches(QKeySequence::Delete) ) {
            ;
        }
        if( event->matches( QKeySequence::Copy ) ) {
            qDebug() << "MyTableWidget::keyPressEvent copy ";
            foreach ( QTableWidgetSelectionRange sr , selectedRanges() ) {
//                qDebug() << sr.topRow() << "," << sr.leftColumn() << ","<< sr.bottomRow() << ","<< sr.rightColumn();
            }

//            qDebug() << " copy " << selectedIndexes();
            copyIndexList = selectedIndexes();
            handled = true;
        }
        if( event->matches( QKeySequence::Paste ) ) {
            qDebug() << "MyTableWidget::keyPressEvent paste ";

            QList<QTableWidgetSelectionRange>  sl = selectedRanges();
            if ( !sl.isEmpty() && !copyIndexList.isEmpty()) {
                int r = sl[0].topRow();
                int c = sl[0].leftColumn();

                int cr = copyIndexList[0].row();
                int cc = copyIndexList[0].column();

                foreach (QModelIndex idx, copyIndexList) {
                    int roff = idx.row() - cr;
                    int coff = idx.column() - cc;
                    quint8 paste_row = r+roff;
                    quint8 paste_col = c+coff;
                    if ( paste_row < rowCount() && paste_col < columnCount() ) {
                        Qt::ItemFlags flags = item (paste_row, paste_col)->flags();
                        MyTableWidgetItem *newItem = new MyTableWidgetItem ();
                        newItem->setText( (item (idx.row(), idx.column()))->text() );
                        newItem->setFlags(flags);
                        if ( flags & Qt::ItemIsEditable )
                            setItem (paste_row, paste_col, newItem);
                    }
                }
            }
            handled=true;
        }
        if ( !handled ) {
            QTableWidget::keyPressEvent(event);
        }
    } else
        QTableWidget::keyPressEvent(event);
}

void MyTableWidget::copy() {
}
void MyTableWidget::paste() {
}


/* ======================================== */

//#define RPM_MIN_FOR_BOOST_CONTROL 2000
//#define RPM_MAX_FOR_BOOST_CONTROL 9000
#define RPM_MIN_FOR_BOOST_CONTROL 0
#define RPM_MAX_FOR_BOOST_CONTROL 9000

N75TableWidget::N75TableWidget(quint8 mode, QWidget *parent) :
    MyTableWidget(parent), mode(mode)
{
    if (mode==0) {
        setToolTip("low boost");
    } else {
        setToolTip("high boost");
    }

    dutyOb = new ColorOverBlend(Qt::cyan, Qt::yellow, Qt::red, 0, 128, 255);
    setpointOb = new ColorOverBlend(Qt::cyan, Qt::yellow, Qt::red, 0, 1.2, 2.0);

    setRowCount(12);
    QStringList rl;

#if defined (Q_WS_MAEMO_5) || defined(ANDROID)
    rl.append("1. dc");
    rl.append("1. bst");
    rl.append("2. dc");
    rl.append("2. bst");
    rl.append("3. dc");
    rl.append("3. bst");
    rl.append("4. dc");
    rl.append("4. bst");
    rl.append("5. dc");
    rl.append("5. bst");
    rl.append("6. dc");
    rl.append("6. bst");
    horizontalHeader()->setStyleSheet("QHeaderView { font-size: 11pt; }");
#else
    rl.append("1. duty [0-255]");
    rl.append("1. boost setpoint [bar]");
    rl.append("2. duty [0-255]");
    rl.append("2. boost setpoint [bar]");
    rl.append("3. duty [0-255]");
    rl.append("3. boost setpoint [bar]");
    rl.append("4. duty [0-255]");
    rl.append("4. boost setpoint [bar]");
    rl.append("5. duty [0-255]");
    rl.append("5. boost setpoint [bar]");
    rl.append("6. duty [0-255]");
    rl.append("6. boost setpoint [bar]");
#endif
    setVerticalHeaderLabels(rl);

    for ( int i = 0 ; i < rowCount() ; i++ )
        setRowHeight(i, 20);

    setColumnCount(16);
    for ( int i = 0 ; i < columnCount() ; i++ ) {
//        qDebug() << "col " << i << " width=" << columnWidth(i);
        setColumnWidth(i, 100);
    }

    QStringList l;
//    int rpm = RPM_MIN_FOR_BOOST_CONTROL;
    for (int i=0 ; i < 16 ; i++) {
//        if ( i > 0 )
//            rpm = rpm + ( (RPM_MAX_FOR_BOOST_CONTROL-RPM_MIN_FOR_BOOST_CONTROL) /16);
        // Calculate rpm this way instead to avoid accumulating rounding errors.
        int rpm = RPM_MIN_FOR_BOOST_CONTROL + ( i*(RPM_MAX_FOR_BOOST_CONTROL-RPM_MIN_FOR_BOOST_CONTROL) /16);
        l.append( QString::number(rpm) );
        setColumnWidth(i, 40);
    }
    setHorizontalHeaderLabels(l);
    setAcceptDrops(true);

    QFont fnt;
    fnt.setPointSize(8);
    fnt.setFamily("Arial");

    for ( int i = 0 ; i < rowCount() ; i++ ) {
        for ( int j = 0 ; j < columnCount() ; j++ ) {
//            MyTableWidgetItem *item = new MyTableWidgetItem();
//            item->setFont(fnt);
//            setItem(i,j, item);
            setItem(i,j, new MyTableWidgetItem());
         }
    }

    setAlternatingRowColors(true);

    connect ( this, SIGNAL(cellChanged(int,int)), this, SLOT(cellChanged(int,int)) );

    resizeColumnsToContents();
}

N75TableWidget::~N75TableWidget () {
    if ( dutyOb )
        delete dutyOb;
    if ( setpointOb )
        delete setpointOb;
}

void N75TableWidget::copy() {

}

void N75TableWidget::paste() {

}

void N75TableWidget::cellChanged(int r,int c) {
    qDebug() << "N75TableWidget::cellChanged r=" << r << " c=" << c;
    QTableWidgetItem* cell = item(r,c);
    if ( cell == NULL )
        return;

    QColor color;
    if ( r == 0 || r == 2 || r == 4 || r == 6 || r == 8 || r == 10 ) {
        //duty
        if ( cell->text().toDouble() < 0 )
            cell->setText(0);
        if ( cell->text().toDouble() > 255 )
            cell->setText("255");
        color = dutyOb->overblend3( cell->text().toDouble() );
    } else {
        //boost
        if ( cell->text().toDouble() < 0 )
            cell->setText(0);
        if ( cell->text().toDouble() > 5 )
            cell->setText("5");
        color = setpointOb->overblend3( cell->text().toDouble() );
    }
    if ( color.isValid() )
        cell->setBackground ( QBrush(color) );
}

/* ***************************************** */


MyTableWidgetItem::MyTableWidgetItem ()
    : QTableWidgetItem() {
}

void MyTableWidgetItem::setData ( int role, const QVariant & value ) {
    if ( role == Qt::EditRole ) {
        bool conv = false;
        value.toDouble( &conv );
        if ( conv )
            QTableWidgetItem::setData( role, value );
        else
            qDebug() << "MyTableWidgetItem::setData not a double!";
    } else {
        QTableWidgetItem::setData( role, value );
    }
}
