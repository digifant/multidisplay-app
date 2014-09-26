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

#ifdef Q_WS_ANDROID
#include <QAndroidStyle>
#endif

#include <cxxabi.h>
#include <iostream>

int main(int argc, char *argv[]) {

    try {
#ifdef Q_WS_MAEMO_5
        QApplication::setStyle(QString("Plastique"));
#endif

#ifdef Q_WS_ANDROID
        QApplication::setStyle(new QAndroidStyle());
#endif

        QApplication a(argc, argv);

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
