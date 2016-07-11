/*
 * Copyright (C) Maurin
 *
 * This file is part of paparazzi
 *
 */
/**
 * @file "modules/meteo/wind_estimator.h"
 * @author Maurin
 * Wind Estimator
 */

#ifndef WIND_ESTIMATOR_H
#define WIND_ESTIMATOR_H

#include "std.h"
#include "math/pprz_algebra_float.h"
#include "math/pprz_geodetic_float.h"

/* Different variable about the data size*/
#define NBR_DATA 17     //Number of data to recive
#define NBR_ANSWER 6    //Number of data to answer

/* Structure of the data to storage before to calcul*/
struct Data_struc_Wind {
  struct FloatRates rates;    ///< body rates (rad/s)
  struct NedCoor_f accel;     ///< body accel (m/s^2) (to be changed)
  struct NedCoor_f Zk_V;      ///< ground speed (m/s)
  float  Zk_Va;               ///< airspeed norm (m/s)
  float  Zk_AOA;              ///< angle of attack (rad)
  struct FloatQuat q;         ///< quaternion
  float  phi;                 ///< roll angle (rad) (to be removed)
  float  theta;               ///< pitch angle (rad) (to be removed)
};

/* Union to convert the Structure to a float array */
union Data_State_Wind {
  struct Data_struc_Wind storage;
  float storage_tab_float[NBR_DATA];
};
extern union Data_State_Wind Data_State_Wind;

/* Structure of the data to recive from the calcul */
struct Answer_storage_Wind {
  float u;    ///< airspeed in x body axis
  float v;    ///< airspeed in y body axis
  float w;    ///< airspeed in z body axis
  float wx;   ///< wind x (north)
  float wy;   ///< wind y (east)
  float wz;   ///< wind z (down)
};

/* Union to generate a array of float link to the data to send */
union Answer_State_Wind {
  struct Answer_storage_Wind storage;
  float storage_tab_float[NBR_ANSWER];
};
extern union Answer_State_Wind Answer_State_Wind;

extern bool wind_estimator_reset;

extern void wind_estimator_init(void);
extern void wind_estimator_periodic(void);
extern void wind_estimator_event(void);

#endif

