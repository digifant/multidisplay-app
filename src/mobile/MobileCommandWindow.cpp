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

#include "MobileCommandWindow.h"
#include "ui_MobileCommandWindow.h"

MobileCommandWindow::MobileCommandWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MobileCommandWindow)
{
    ui->setupUi(this);

#ifdef Q_WS_MAEMO_5
        setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
}

MobileCommandWindow::~MobileCommandWindow()
{
    delete ui;
}
