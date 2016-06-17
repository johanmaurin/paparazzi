/*
 * Copyright (C) Maurin
 *
 * This file is part of paparazzi
 *
 */
/**
 * @file "modules/meteo/wind_estimator.c"
 * @author Maurin
 * Wind Estimator
 */

#include "modules/meteo/wind_estimator.h"
#include "lib_ukf_wind_estimator/calculate_wind.h"

#include <ch.h>
#include <hal.h>
#include "main_chibios.h"
#include "modules/loggers/sdlog_chibios/sdLog.h"
#include "modules/loggers/sdlog_chibios/usbStorage.h"
#include "modules/loggers/sdlog_chibios.h"
#include "mcu_periph/adc.h"
#include "led.h"

/*
 * Start log thread
 */
static THD_WORKING_AREA(wa_thd_windestimation, 2048);
static __attribute__((noreturn)) void thd_windestimate(void *arg);

/*------------------get_data_from_state---------------*/
/*  Fonction put data from state to Data_State        */
/*----------------------------------------------------*/
void get_data_from_state(void){	
	Data_State.storage.omega = *stateGetBodyRates_f(); //may be wrong
	Data_State.storage.omega_A = *stateGetAccelEcef_f();
	Data_State.storage.Zk_V = *stateGetSpeedNed_f();
	Data_State.storage.Zk_Va = stateGetHorizontalSpeedNorm_f();
	Data_State.storage.Zk_AOA = stateGetAngleOfAttack_f();
	Data_State.storage.q = *stateGetNedToBodyQuat_f();
	Data_State.storage.phi = stateGetNedToBodyEulers_f()->phi;
	Data_State.storage.theta = stateGetNedToBodyEulers_f()->phi;
}

void put_data_in_state(void){
	//need to know where   to put data
}

void wind_estimator_init(void)
{
  init_calculator();
  // Start log thread
  chThdCreateStatic (wa_thd_windestimation, sizeof(wa_thd_windestimation),
      NORMALPRIO+2, thd_windestimate, NULL);
}

static void thd_windestimate(void *arg)
{
  (void) arg;
  chRegSetThreadName("start wind estimation");
  
  while (TRUE){
	  
	  get_data_from_state();
	  parse_data_for_wind_estimation();
	  get_wind();
	  put_data_in_state();
	  
	  
	  
  }

  // Wait before starting the log if needed
  chThdSleepSeconds (SDLOG_START_DELAY);
}

