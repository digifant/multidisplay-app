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

#include "multidisplayuimainwindow.h"
#include "AppEngine.h"

#include <QtGui>
#include <QApplication>
#include <QDebug>
#include <QWidget>

#include <cxxabi.h>
#include <iostream>

int main(int argc, char *argv[]) {

    try {
#ifdef Q_WS_MAEMO_5
        QApplication::setStyle(QString("Plastique"));
#endif

#ifdef DIGIFANTAPP
        QApplication::setOrganizationName("gummel-informatics");
        QCoreApplication::setApplicationName("digifantview");
        QSettings settings;
        settings.setValue ("md/md", QVariant(false));
#else
        QApplication::setOrganizationName("MultiDisplay");
        QCoreApplication::setApplicationName("UI");
        QSettings settings;
        settings.setValue ("md/md", QVariant(true));
#endif
        QApplication a(argc, argv);
        //dont works
        //a.setStyleSheet( "GLGauge {color:black}");

#if defined ( Q_OS_ANDROID)
        {
        //FIX android dark holo theme default has light background and text color! -> set background to black!
        QColor fr = a.palette().color(QPalette::Window);
        //light 243 dark 0
        //QPushButton { color: yellow; }
        auto teststyle = R"(
                QWidget { background-color: black; }
                QComboBox { background-color: black }

                QPushButton { background-color: black }
                       )";
        //qDebug() << fr.lightness();
        if ( fr.lightness() == 0 ) {
            a.setStyleSheet(teststyle);
        }
    }
#endif



        AppEngine *e = AppEngine::getInstance();
        e->show();

//#ifndef Q_WS_MAEMO_5
        //Windows / Linux Desktop GUI
    //    MultidisplayUIMainWindow mw;
    //    mw.show();
//#else
        //Maemo Stacked Windows
//#endif

        return a.exec();

    } catch ( std::exception e ) {
        //__gnu_cxx::recursive_init_error
        std::cout << "exception " << " what: " << e.what() << std::endl;
    }
}
