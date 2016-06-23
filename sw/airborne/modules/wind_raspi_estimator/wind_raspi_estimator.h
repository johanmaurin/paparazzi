/*
 * Copyright (C) Maurin
 *
 * This file is part of paparazzi
 *
 * paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with paparazzi; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
/**
 * @file "modules/wind_raspi_estimator/wind_raspi_estimator.h"
 * @author Maurin
 * Wind Raspberry Pi Estimation
 */

#ifndef WIND_EST
#define WIND_EST

#include "state.h"
#include "autopilot.h"
#include "subsystems/datalink/datalink.h"
#include "subsystems/datalink/downlink.h"

#include "subsystems/gps.h"
#include "math/pprz_algebra_float.h"
#include "std.h"
#include "mcu_periph/gpio.h"
#include "generated/airframe.h"
#include "modules/datalink/extra_pprz_dl.h"

/*--------------Variable for data----------*/
#define NBR_DATA 17
#define NBR_ANSWER 5
/*--------------Struture of the data to send to the raspberry----------*/
struct Data_struc{
	struct FloatRates omega;
/*	float  omegaa_p;
	float  omegaa_q;
	float  omegaa_r;*/
	struct NedCoor_f omega_A;
/*	float  omegaa_Ax;
	float  omegaa_Ay;
	float  omegaa_Az;*/
	struct NedCoor_f Zk_V;
/*	float  Zk_vx;
	float  Zk_vy;
	float  Zk_vz;*/
	float  Zk_Va;
	float  Zk_AOA;
	struct FloatQuat q;
/*	float  q1;
	float  q2;
	float  q3;
	float  q4;*/
	float  phi;
	float  theta;
};
/*--------------Union to convert the Structure to a float array----------*/
union Data_State{
	struct Data_struc storage;
	float storage_tab_float[NBR_DATA];
};
extern union Data_State Data_State;
/*--------------Structure of the data to recive from the raspberry----------*/
struct Answer_struct{
  float ac_id;       
  float pad0;
  float east;
  float north;
  float airspeed;                
};
/*--------------Union to convert the Structure to float array----------*/
union Answer_State{
	struct Answer_struct storage;
	float storage_tab_float[NBR_ANSWER];
};
extern union Answer_State Answer_State;

extern void init_extra_pprz_raspi(void);
extern int storage_data_from_state(void);
extern void send_data_to_wind_estimation(void);
extern void get_wind_from_wind_estimation(void);

#endif
