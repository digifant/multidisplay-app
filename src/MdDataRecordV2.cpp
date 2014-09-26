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

#include "MdDataRecordV2.h"
#include "QDebug"

namespace compatibility {

MdDataRecordV2::MdDataRecordV2() {
        sensorR = new MdSensorRecordV2();}

MdDataRecordV2::~MdDataRecordV2() {
        if ( sensorR ) {
                        delete sensorR;
                        sensorR = NULL;
        }
}

QDataStream& operator>> (QDataStream& s, compatibility::MdDataRecordV2 *d) {
        s >> (MdSensorRecordV2*) d->sensorR;
        return s;
}


MdSensorRecordV2::MdSensorRecordV2 () {
}
MdSensorRecordV2::~MdSensorRecordV2 () {
}

MdSensorRecordV2::MdSensorRecordV2 ( int time, int rpm, int throttle, double boost, double lambda, double lmm,
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
                 quint8 df_cold_startup_enrichment, quint8 df_warm_startup_enrichment, quint8 df_ect_startup_enrichment,
                 quint8 df_acceleration_enrichment, quint8 df_counter_startup_enrichment, quint8 df_iat_startup_enrichment,
                 quint8 df_ignition_addon_counter, quint8 df_igniton_addon, quint8 df_ect_injection_addon,
                 quint8 df_isv, quint8 df_lc_flags,
                 double df_ignition_total_retard, double df_ect, double df_iat, double df_ignition
                 ) :
    MdSensorRecord::MdSensorRecord ( time, rpm, throttle, boost, lambda, lmm,
                     casetemp, agt0, agt1, agt2, agt3, agt4,
                     agt5, agt6, agt7, batcur,
                     VDOPres1, VDOPres2, VDOPres3,
                     VDOTemp1, VDOTemp2, VDOTemp3,
                     speed, gear, n75, n75_req_boost, n75_req_boost_pwm, flags,
                     0,
                     df_boost_raw, df_lambda, df_knock_raw, df_ect_raw, df_iat_raw,
                     df_co_poti, df_flags, df_ign,
                     df_cyl1_knock_retard,  df_cyl1_knock_decay,  df_cyl2_knock_retard,
                     df_cyl2_knock_decay,  df_cyl3_knock_retard,  df_cyl3_knock_decay,  df_cyl4_knock_retard,
                     df_cyl4_knock_decay,  df_voltage_raw, df_inj_time,
                     df_cold_startup_enrichment,  df_warm_startup_enrichment,  df_ect_startup_enrichment,
                     df_acceleration_enrichment,  df_counter_startup_enrichment,  df_iat_startup_enrichment,
                     df_ignition_addon_counter,  df_igniton_addon,  df_ect_injection_addon,
                     0, df_isv,  df_lc_flags,
                     df_ignition_total_retard, df_ect, df_iat, df_ignition, 0,
                     0, 0, 0
                     ) {
    }


QDataStream& operator>> (QDataStream& s, MdSensorRecordV2 *r) {
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
    s >> r->VDOPres1;
    s >> r->VDOPres2;
    s >> r->VDOPres3;
    s >> r->VDOTemp1;
    s >> r->VDOTemp2;
    s >> r->VDOTemp3;
    s >> r->speed;
    s >> r->gear;
    s >> r->n75;
    s >> r->n75_req_boost;
    s >> r->n75_req_boost_pwm;
    s >> r->flags;

    s >> r->df_boost_raw;
    s >> r->df_lambda;
    s >> r->df_knock_raw;
    s >> r->df_ect_raw;
    s >> r->df_iat_raw;
    s >> r->df_co_poti;
    s >> r->df_flags;
    s >> r->df_ign;
    quint8 unused;
    s >> unused;
    s >> r->df_cyl1_knock_retard;
    s >> r->df_cyl1_knock_decay;
    s >> r->df_cyl2_knock_retard;
    s >> r->df_cyl2_knock_decay;
    s >> r->df_cyl3_knock_retard;
    s >> r->df_cyl3_knock_decay;
    s >> r->df_cyl4_knock_retard;
    s >> r->df_cyl4_knock_decay;
    s >> unused;
    s >> r->df_voltage_raw;
    s >> r->df_inj_time;
    s >> r->df_cold_startup_enrichment;
    s >> r->df_warm_startup_enrichment;
    s >> r->df_ect_enrichment;
    s >> r->df_acceleration_enrichment;
    s >> r->df_counter_startup_enrichment;
    s >> r->df_iat_enrichment;
    s >> r->df_ignition_addon_counter;
    s >> r->df_igniton_addon;
    s >> r->df_ect_injection_addon;
    r->df_rpm_delta_hall = 0;
    s >> r->df_isv;
    s >> r->df_lc_flags;

    s >> r->df_ignition_total_retard;
    s >> r->df_ect;
    s >> r->df_iat;
    s >> r->df_ignition;

    return s;
}



} //namespace
