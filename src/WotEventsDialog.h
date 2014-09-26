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


#ifndef WOTEVENTSDIALOG_H
#define WOTEVENTSDIALOG_H

#include <QDialog>

namespace Ui {
class WotEventsDialog;
}

class WotEventsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit WotEventsDialog(QWidget *parent = 0);
    ~WotEventsDialog();

    void show ( QList<int> idxL );
    void showKnock ( QList<int> idxL );
    void showEGT ( QList<int> idxL );
    void show ( QMap<QString, QMap<QString, QVariant> > &d );

signals:
    void jumpToDataIdx (int);

protected slots:
    void jumpTo ();
    void cellDoubleClicked ( int row, int column );

private:
    Ui::WotEventsDialog *ui;
    QList<int> idxList;
};

#endif // WOTEVENTSDIALOG_H
