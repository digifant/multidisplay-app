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

#ifndef SERIALOPTIONS_H
#define SERIALOPTIONS_H

#include <qglobal.h>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QDialog>
#else
    #include <QtGui/QDialog>
#endif

#include "ui_serialoptions.h"

class SerialOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    SerialOptionsDialog(QWidget *parent = 0);
    ~SerialOptionsDialog();

    Ui::SerialOptionsDialog* getUi();

    void showEvent ( QShowEvent * event );

private:
    Ui::SerialOptionsDialog ui;
};

#endif // SERIALOPTIONS_H
