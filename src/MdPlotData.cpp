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


#include "MdPlotData.h"

#include <QDebug>
#include <cmath>

iResult::iResult() {
    curve = NULL;
}

iResultList::~iResultList () {
    foreach (iResult* i, resultList)
        delete i;
}


MdPlotData::MdPlotData (int windowMark, int windowSize) : cleanCounter(0), xRange(xRange), windowMark(windowMark), windowSize(windowSize), windowBegin(0), windowEnd(0) {
    xData.clear();
}

MdPlotData::MdPlotData () : cleanCounter(0), xRange(0), windowMark(-1), windowSize(-1), windowBegin(0), windowEnd(0) {
    xData.clear();
}

MdPlotData::MdPlotData(double xRange) : cleanCounter(0), xRange(xRange) {
    xData.clear();
}

//MyPlotData::MyPlotData ( const MyPlotData& d) {
//	cleanCounter = d.cleanCounter;
//	xRange = d.xRange;
//	xData = d.xData;
//	yData = d.yData;
//}

MdPlotData::~MdPlotData() {
}



void MdPlotData::append (double x, double y) {
    xData.append(x);
    yData.append(y);
    adjustWindow();

    //	cleanCounter++;
    //	//TODO
    //	if ( (windowSize > -1) && (cleanCounter > 100) ) {
    //		//XXX Hack. we should instead adjust begin / end of plottet data!!!
    //		cleanXLowerAs( xData.last() - xRange);
    //		cleanCounter = 0;
    //	}
}

void MdPlotData::setWinSize(const int &nws) {
    windowSize = nws;
    adjustWindow();
}

void MdPlotData::setWinMarkMicroRelative(const int &quotient, const bool &left, const int &maxMark) {
    //max is the beginning!
    int step = ( round ( (double) xData.size() / (double) maxMark) ) / quotient;
    if ( left )
        windowMark += step;
    else
        windowMark -= step;

    if ( windowMark >= xData.size() )
        windowMark = xData.size() -1;
    else if ( windowMark <= 0 )
        windowMark = 0;
    adjustWindow();
}

void MdPlotData::setWinMark(const int &nwm, const int &maxMark) {
    //max is the beginning!
    windowMark = ( round ( (double) xData.size() / (double) maxMark )  * nwm );
    if ( windowMark >= xData.size() )
        windowMark = xData.size() -1;

//    qDebug() << "nwm=" << nwm << " maxMark=" << maxMark << " windowMark=" << windowMark;

    adjustWindow();
}

bool MdPlotData::interpolateSearch ( const double xval, iResult &res ) {
    if (xData.isEmpty())
        return false;

    res.x = xval;

    int min = 0;
    int max = xData.size() - 1;
    int mid = 0;

    while ( xData[mid]!=xval && min < max ) {
        mid = (min + max) / 2;
        if ( xval > xData[mid] )
            min = mid + 1;
        else
            max = mid - 1;
    }
    if ( xData[mid]==xval ) {
        res.needInterPolation = false;
        res.l = mid;
        res.r = mid;
        //		qDebug() << "interpolateSearch found l=" << res.l << " r=" << res.r;
        return true;
    } else if ( min == max ) {
        res.needInterPolation = true;
        if (xData[min]>xval) {
            res.l = min-1;
            res.r = min;
        } else {
            res.l = min;
            res.r = min + 1;
        }
        //		qDebug() << "interpolateSearch found l=" << res.l << " r=" << res.r;

        //boundary check
        if ( res.r >= xData.size() ) {
            res.r = xData.size()-1;
            qDebug() << "interpolateSearch failed boundary check for r!";
            return false;
        } else if ( res.l < 0 ) {
            res.l = 0;
            qDebug() << "interpolateSearch failed boundary check for l!";
            return false;
        }
        return true;
    } else {
        if (min < max)
            qDebug() << "interpolateSearch case min < max: handle me";
        if (min > max) {
            res.l = max;
            res.r = min;
            //boundary check
            if ( res.r >= xData.size() ) {
                res.r = xData.size()-1;
                qDebug() << "interpolateSearch failed boundary check for r!";
                return false;
            } else if ( res.l < 0 ) {
                res.l = 0;
                qDebug() << "interpolateSearch failed boundary check for l!";
                return false;
            }
            return true;
        }
    }
    return false;
}

double MdPlotData::getInterpolatedYValue ( int lindex, int rindex, double xval ) {
    //http://en.wikipedia.org/wiki/Linear_interpolation
    if ( (lindex < 0) || (lindex >= xData.size()) || (rindex >= xData.size()) )
        return 0;
    double x0 = xData[lindex];
    double y0 = yData[lindex];
    double x1 = xData[rindex];
    double y1 = yData[rindex];
    if ( x1-x0 < 0 )
        qDebug () << "lindex=" << lindex << " rindex=" << rindex << " x0=" << x0 << " x1=" << x1;
    return y0 + (xval - x0) * (y1-y0)/(x1-x0);
}

void MdPlotData::adjustWindow () {
    windowEnd = xData.size() - 1 - windowMark;
    if ( windowEnd < 0 ) {
        windowEnd = xData.size() - 1;
        if ( windowEnd < 0 )
            windowEnd = 0;
    }
    windowBegin = windowEnd - windowSize;
    if ( windowBegin <= 0 ) {
        if ( xData.size() > windowSize )
            windowEnd = windowSize; //show at least the fill windowSize
        windowBegin = 0;
    }
//    qDebug() << "adjustWin Mark=" << windowMark << " size=" << windowSize << " Begin=" << windowBegin << " windowEnd=" << windowEnd;
    //fails
    //qDebug() << " x=" << xData[windowEnd] << " x=" << xData[windowBegin];
}

double MdPlotData::x(size_t i) const {
    //	return xData[xData.size()-i-1];
    if ( windowBegin + i < xData.size()-1 )
        return xData[windowBegin + i];
    return -666.0;
}

QPointF MdPlotData::sample(size_t i) const {
    //	return yData[yData.size()-i-1];
    if ( windowBegin + i < yData.size()-1 )
        return QPointF (xData[windowBegin + i], yData[windowBegin + i]);
    return QPointF (-666.0, -666);
}

void MdPlotData::cleanXLowerAs (double xDel) {
    //XXX Hack. we should instead adjust begin / end of plottet data!!!
    if ( xDel > 0 ){
        int a = 0;
        for ( int i = 0 ; i < xData.size() && xData[i]<xDel ; i++ ) {
            xData.remove(i);
            yData.remove(i);
            a++;
        }
        qDebug() << "cleanXLowerAs: deleted " << a << " (xDel=" << xDel << ", cleanC=" << cleanCounter << " *(xData.end())=" << *(xData.end()) << " xRange=" << xRange;
    }
}

void MdPlotData::clear () {
    xData.clear();
    yData.clear();
    cleanCounter=0;
    windowBegin=0;
    windowEnd=0;
    windowMark=0;
    //        windowSize=0;
}



QRectF MdPlotData::boundingRect() const {
    if ( xData.size() > 0 && windowEnd < xData.size() ) {
        //		qDebug() << "boundingRect() xData[windowBegin]=" << xData[windowBegin] << " xData[windowEnd]=" << xData[windowEnd];
        return QRectF (xData[windowBegin], 0, xData[windowEnd] - xData[windowBegin], 0);
    }
    //	qDebug() << "empty boudingRect " << xData.size() << " " << windowEnd;
    //	return QwtDoubleRect ();
    return QRectF(1.0, -1.0, 1.0, -1.0);
}



size_t MdPlotData::size() const {
    if ( windowSize < xData.size()-1 )
        return windowSize;
    return windowEnd - windowBegin;
}

int MdPlotData::getWindowBegin() {
    return windowBegin;
}

QVector<double> MdPlotData::x() const {
    return xData;
}

QVector<double> MdPlotData::y() const {
    return yData;
}
