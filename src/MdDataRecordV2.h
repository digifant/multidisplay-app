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

#ifndef MdDataRecordV2_H_
#define MdDataRecordV2_H_

#include "MdData.h"
#include "Map16x1.h"

namespace compatibility {

class MdDataRecordV2 : public MdDataRecord {
        friend QDataStream& operator>> (QDataStream& s, MdDataRecordV2 *d);

public:
        MdDataRecordV2();
        virtual ~MdDataRecordV2();
};

QDataStream& operator>> (QDataStream& s, MdDataRecordV2 *d);


/**
  * data version 3 (used until 2013-04)
  */
class MdSensorRecordV2 : public MdSensorRecord {
        friend QDataStream& operator>> (QDataStream& s, MdSensorRecordV2 *d);

public:
        MdSensorRecordV2();

        MdSensorRecordV2 ( int time, int rpm, int throttle, double boost, double lambda, double lmm,
                         double casetemp, double agt0, double agt1, double agt2, double agt3, double agt4,
                         double agt5, double agt6, double agt7, double batcur,
                         double VDOPres1, double VDOPres2, double VDOPres3,
                         double VDOTemp1, double VDOTemp2, double VDOTemp3,
                         int speed, int gear, int n75, double n75_req_boost, quint8 n75_req_boost_pwm, quint8 flags,
                         quint8 df_boost_raw, quint8 df_lambda, quint8 df_knock_raw, quint8 df_ect_raw, quint8 df_iat_raw,
                         quint8 df_co_poti, quint8 df_flags, quint8 df_ign,
                         quint8 df_rpm_map, quint8 df_cyl1_knock_retard, quint8 df_cyl1_knock_decay, quint8 df_cyl2_knock_retard,
                         quint8 df_cyl2_knock_decay, quint8 df_cyl3_knock_retard, quint8 df_cyl3_knock_decay, quint8 df_cyl4_knock_retard,
                         quint8 df_cyl4_knock_decay, quint8 df_sci_counter, quint8 df_voltage_raw, quint16 df_inj_time,
                         quint8 df_cold_startup_enrichment, quint8 df_warm_startup_enrichment, quint8 df_ect_enrichment,
                         quint8 df_acceleration_enrichment, quint8 df_counter_startup_enrichment, quint8 df_iat_enrichment,
                         quint8 df_ignition_addon_counter, quint8 df_igniton_addon, quint8 df_ect_injection_addon, quint8 df_isv, quint8 df_lc_flags,
                         double df_ignition_total_retard, double df_ect, double df_iat, double df_ignition
                         );

        virtual ~MdSensorRecordV2();

};
QDataStream& operator>> (QDataStream& s, MdSensorRecordV2 *r);


} // namespace


#endif /* MdDataRecordV2_H_ */
