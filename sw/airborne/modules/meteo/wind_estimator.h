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
#include "lib_ukf_wind_estimator/calculate_wind.h"
/*----------------------------Union-------------------------------------------*/
union Data_struc_State{
	struct Data_struc_Wind storage;
	float storage_tab_float[NBR_DATA];
};
extern union Data_struc_State Data_struc_State;

union Answer_State_State{
	struct Answer_storage_Wind storage;
	float storage_tab_float[NBR_ANSWER];
};
extern union Answer_State_State Answer_State_State;
/*----------------------------------------------------------------------------*/
extern void wind_estimator_init(void);
extern void get_data_from_state(void);
extern void put_data_to_State(void);

#endif

