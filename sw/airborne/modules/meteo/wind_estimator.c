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
#include "std.h"
//#include "ff.h"
#include "pprzlink/pprzlink_device.h"
#include "state.h"
#include <ch.h>
#include <hal.h>
#include "main_chibios.h"
#include "modules/loggers/sdlog_chibios/sdLog.h"
#include "modules/loggers/sdlog_chibios/usbStorage.h"
#include "modules/loggers/sdlog_chibios.h"
#include "wind_estimator.h"
#include <time.h>  

#define PERIODE_WIND_ESTIMATION 0.02 

union Data_State Data_State;
union Answer_State Answer_State;
	
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
  clock_t t;
  float t_s;
  float time_to_wait;

    
  
  while (TRUE){
	  t = clock(); 
	  
	  get_data_from_state();
	  parse_data_for_wind_estimation();
	  get_wind();
	  put_data_in_state();
	  
	  
	  t = clock() - t;   
	  t_s =(((float)t)/CLOCKS_PER_SEC);
	  time_to_wait = t_s - PERIODE_WIND_ESTIMATION;
	  if(time_to_wait>0){
		  chThdSleepMilliseconds(time_to_wait*1000);
	  }
	  
	  //chThdWaitUntil();
  }
}

