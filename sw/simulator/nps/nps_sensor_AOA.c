/*
 * Copyright (C) 2014 Felix Ruess <felix.ruess@gmail.com
 *
 * This file is part of paparazzi.
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
 * along with paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * @file nps_sensor_AOA.c
 *
 * Simulated Angle of Attack of the Wind for NPS simulator.
 *
 */

#include "nps_sensor_AOA.h"

#include "generated/airframe.h"

#include "std.h"
#include "nps_fdm.h"
#include "nps_random.h"
#include NPS_SENSORS_PARAMS

/// 10Hz default
#ifndef NPS_AOA_DT
#define NPS_AOA_DT 0.01
#endif

/// standard deviation in radian (default 0.1 rad)
#ifndef NPS_AOA_NOISE_STD_DEV
#define NPS_AOA_NOISE_STD_DEV 0.0001
#endif

#ifndef NPS_AOA_OFFSET
#define NPS_AOA_OFFSET 0
#endif


void nps_sensor_AOA_init(struct NpsSensorAOA *AOA, double time)
{
  AOA->value = 0.;
  AOA->offset = NPS_AOA_OFFSET;
  AOA->noise_std_dev = NPS_AOA_NOISE_STD_DEV;
  AOA->next_update = time;
  AOA->data_available = FALSE;
}


void nps_sensor_AOA_run_step(struct NpsSensorAOA *AOA, double time)
{

  if (time < AOA->next_update) {
    return;
  }

  /* equivalent airspeed + sensor offset */
  AOA->value = fdm.AOA + AOA->offset;
  /* add noise with std dev rad */
  AOA->value += get_gaussian_noise() * AOA->noise_std_dev;
  /* can't bet more than PI */
  if( M_PI < AOA->value){
	  AOA->value =0;
  }	

  AOA->next_update += NPS_AOA_DT;
  AOA->data_available = TRUE;
}
