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


#ifndef VISCONFIGDIALOG_H
#define VISCONFIGDIALOG_H

#include <QDialog>

#include <QMap>
#include <QString>
#include <qwt_plot_curve.h>
#include "ui_visconfigdialog.h"

class VisConfigDialog : public QDialog
{
    Q_OBJECT

public:
    VisConfigDialog(QWidget *parent = 0);
    ~VisConfigDialog();

    void readConfig (QMap<QString, QwtPlotCurve*> cfgMap);
    QTableWidget* getCfgTableWidget ();

public slots:
	void cellChanged (int row, int column);


private:
    Ui::VisConfigDialogClass ui;
};

#endif // VISCONFIGDIALOG_H
