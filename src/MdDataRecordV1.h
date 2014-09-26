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

#ifndef MDDATARECORDV1_H_
#define MDDATARECORDV1_H_

#include "MdData.h"

namespace compatibility {

class MdDataRecordV1 : public MdDataRecord {
	friend QDataStream& operator>> (QDataStream& s, MdDataRecordV1 *d);

public:
	MdDataRecordV1();
	virtual ~MdDataRecordV1();
};

QDataStream& operator>> (QDataStream& s, MdDataRecordV1 *d);



class MdSensorRecordV1 : public MdSensorRecord {
	friend QDataStream& operator>> (QDataStream& s, MdSensorRecordV1 *d);

public:
	MdSensorRecordV1();

	MdSensorRecordV1 ( int time, int rpm, int throttle, double boost, double lambda, double lmm,
			double casetemp, double agt0, double agt1, double batcur );

	virtual ~MdSensorRecordV1();
};
QDataStream& operator>> (QDataStream& s, MdSensorRecordV1 *r);


} // namespace


#endif /* MDDATARECORDV1_H_ */
