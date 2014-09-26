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

#include "MdDataRecordV1.h"

namespace compatibility {

MdDataRecordV1::MdDataRecordV1() {
	sensorR = new MdSensorRecordV1();}

MdDataRecordV1::~MdDataRecordV1() {
	if ( sensorR ) {
			delete sensorR;
			sensorR = NULL;
	}
}

QDataStream& operator>> (QDataStream& s, compatibility::MdDataRecordV1 *d) {
	s >> (MdSensorRecordV1*) d->sensorR;
	return s;
}


MdSensorRecordV1::MdSensorRecordV1 () {
}
MdSensorRecordV1::~MdSensorRecordV1 () {
}

MdSensorRecordV1::MdSensorRecordV1 ( int time, int rpm, int throttle, double boost,
		double lambda, double lmm,
		double casetemp, double agt0, double agt1, double batcur ) {
        MdSensorRecord ( time, rpm, throttle, boost, lambda, lmm, casetemp, agt0, agt1, 0, 0, 0, 0, 0, 0, batcur,
                        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0,
                        0, 0 );
}


QDataStream& operator>> (QDataStream& s, MdSensorRecordV1 *r) {
    s >> r->time;
    s >> r->rpm;
    s >> r->throttle;
    s >> r->boost;
    s >> r->lambda;
    s >> r->lmm;
    s >> r->casetemp;
    s >> r->egt[0];
    s >> r->egt[1];
    s >> r->egt[2];
    s >> r->egt[3];
    s >> r->egt[4];
    s >> r->egt[5];
    s >> r->egt[6];
    s >> r->egt[7];
    s >> r->batcur;
    r->VDOPres1 = 0;
    r->VDOPres2 = 0;
    r->VDOPres3 = 0;
    r->VDOTemp1 = 0;
    r->VDOTemp2 = 0;
    r->VDOTemp3 = 0;
    r->speed = 0;
    r->gear = 0;
    r->n75 = 0;
    return s;
}



} //namespace
