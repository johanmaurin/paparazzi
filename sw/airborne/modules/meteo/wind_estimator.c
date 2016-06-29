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
#include "ff.h"
#include "pprzlink/pprzlink_device.h"
#include "state.h"
#include <ch.h>
#include <hal.h>
#include "main_chibios.h"
#include "modules/loggers/sdlog_chibios/sdLog.h"
#include "modules/loggers/sdlog_chibios/usbStorage.h"
#include "modules/loggers/sdlog_chibios.h"
#include "wind_estimator.h"
/*--------------------Varibales---------------------*/
#define PERIODE_WIND_ESTIMATION_MS 20    //Perido of thread 50Hz = 20ms

static mutex_t write_state_mtx;			//mutex for read in State
static mutex_t read_state_mtx;			//mutex for write in State

static MUTEX_DECL(write_state_mtx);
static MUTEX_DECL(read_state_mtx);

int data_to_State = 0;					//Flag for event to write new data in State

struct FloatVect2 NE_Wind_f;
/* Data using by the Thread */
union Data_struc_State Data_struc_State;
union Answer_State_State Answer_State_State;
/* Data use by the calculator */
union Data_State_Wind Data_State_Wind;
union Answer_State_Wind Answer_State_Wind;
/*----------------------------------------------------*/
/*
 * Start log thread
 */
static THD_WORKING_AREA(wa_thd_windestimation, 2048);
static __attribute__((noreturn)) void thd_windestimate(void *arg);
/*------get_data_from_state_tmp__to_wind_estimator----*/
/*  Fonction put data from Data_struc_State to 		  */
/* 	Data_State_Wind  (Entrety data only)   			  */
/*----------------------------------------------------*/
void get_data_from_state_tmp__to_wind_estimator(void){	
	Data_State_Wind.storage.omega = Data_struc_State.storage.omega; 
	Data_State_Wind.storage.omega_A = Data_struc_State.storage.omega_A;
	Data_State_Wind.storage.Zk_V = Data_struc_State.storage.Zk_V;
	Data_State_Wind.storage.Zk_Va = Data_struc_State.storage.Zk_Va;
	Data_State_Wind.storage.Zk_AOA = Data_struc_State.storage.Zk_AOA;
	Data_State_Wind.storage.q = Data_struc_State.storage.q;
	Data_State_Wind.storage.phi = Data_struc_State.storage.phi;
	Data_State_Wind.storage.theta = Data_struc_State.storage.theta;
}
/*----------------get_data_from_state-----------------*/
/*  Put Data from State in struct use by the Thread   */				
/*----------------------------------------------------*/
void get_data_from_state(void){	
	chMtxLock(&read_state_mtx);
	
	Data_struc_State.storage.omega = *stateGetBodyRates_f(); //   rad/s
	Data_struc_State.storage.omega_A = *stateGetAccelNed_f(); //   m/s^2
	Data_struc_State.storage.Zk_V = *stateGetSpeedNed_f(); //    m/s
	Data_struc_State.storage.Zk_Va =  stateGetAirspeed_f()*cos(stateGetAngleOfAttack_f());    //    m/s  // projection de la norme qur l'axe X (cos sin)
	Data_struc_State.storage.Zk_AOA = stateGetAngleOfAttack_f();   // rad.
	Data_struc_State.storage.q = *stateGetNedToBodyQuat_f();
	Data_struc_State.storage.phi = stateGetNedToBodyEulers_f()->phi; //rad
	Data_struc_State.storage.theta = stateGetNedToBodyEulers_f()->theta; //rad
	chMtxUnlock(&read_state_mtx);
}
/*---------put_data_from_wind_estimator_to_state------*/
/*  Fonction put data from Data_State_Wind to 		  */
/* 	Data_struc_State  (output data only)   		      */
/*----------------------------------------------------*/
void put_data_from_wind_estimator_to_state(void){
	data_to_State=0;
	
	/*Vair : Xout 0*/
	Answer_State_State.storage_tab_float[0] = Answer_State_Wind.storage_tab_float[0]; 
	/*North : Xout 3*/
	Answer_State_State.storage_tab_float[3] = Answer_State_Wind.storage_tab_float[3]; 
	/*East :  Xout 4*/
	Answer_State_State.storage_tab_float[4] = Answer_State_Wind.storage_tab_float[4]; 
	/*Down : Xout 5*/
	Answer_State_State.storage_tab_float[5] = Answer_State_Wind.storage_tab_float[5]; 
	
	data_to_State=1;
}
/*-----------------put_data_to_State------------------*/
/*  Fonction put data in State				 		  */
/*----------------------------------------------------*/
void put_data_to_State(void){
	if(data_to_State==1){
		chMtxLock(&write_state_mtx);
		
		NE_Wind_f.x = Answer_State_State.storage_tab_float[3];
		NE_Wind_f.y = Answer_State_State.storage_tab_float[4];
		
		stateSetAirspeed_f(Answer_State_State.storage_tab_float[0]);			//NEED CHECK
		stateSetHorizontalWindspeed_f(&NE_Wind_f);								//NEED CHECK
		stateSetVerticalWindspeed_f(Answer_State_State.storage_tab_float[5]);   //NEED CHECK
		
	    chMtxUnlock(&write_state_mtx);
		data_to_State=0;
	}
}
/*-----------------wind_estimator_init----------------*/
/*  Init the Thread and the calculator   	 		  */
/*----------------------------------------------------*/
void wind_estimator_init(void)
{
  init_calculator();
  // Start log thread
  chThdCreateStatic (wa_thd_windestimation, sizeof(wa_thd_windestimation),
      NORMALPRIO+2, thd_windestimate, NULL);
}
/*------------------thd_windestimate------------------*/
/*  Thread fonction                      	 		  */
/*----------------------------------------------------*/
static void thd_windestimate(void *arg)
{
  (void) arg;
  chRegSetThreadName("start wind estimation");
  systime_t start;
  systime_t end;

  while (TRUE){
	  start = chVTGetSystemTime(); 
	  end = start + MS2ST(PERIODE_WIND_ESTIMATION_MS);
	  
	  chMtxLock(&read_state_mtx);
	  get_data_from_state();		//critical 
	  chMtxUnlock(&read_state_mtx);
	  
	  parse_data_for_wind_estimation();
	  
	  get_wind_from_wind_estimation();
	  
	  chMtxLock(&write_state_mtx);
	  put_data_from_wind_estimator_to_state();			//critical
	  chMtxUnlock(&write_state_mtx);
	  
	  chThdSleepUntil(end);
  }
}

