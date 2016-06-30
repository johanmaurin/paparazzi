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
#include "lib_ukf_wind_estimator/Gen_UKF.h"
/*--------------------Varibales---------------------*/
static MUTEX_DECL(writeread_state_mtx);
static SEMAPHORE_DECL(readwrite_state_sem, 0);

int data_to_State = 0;					//Flag for event to write new data in State
int first_time=1;

int time_step_before;
int time_calculator_dt;

struct FloatVect2 NE_Wind_f;

/* Data use by the calculator */
union Data_State_Wind Data_State_Wind;
union Answer_State_Wind Answer_State_Wind;
/*----------------------------------------------------*/
/*
 * Start log thread
 */
static THD_WORKING_AREA(wa_thd_windestimation, 11*1024);
static __attribute__((noreturn)) void thd_windestimate(void *arg);
/*----------------get_data_from_state-----------------*/
/*  Put Data from State in struct use by the Thread   */				
/*----------------------------------------------------*/
void get_data_from_state(void){	
	if(chMtxTryLock(&writeread_state_mtx)){
		Data_State_Wind.storage.omega = *stateGetBodyRates_f(); //   rad/s
		Data_State_Wind.storage.omega_A = *stateGetAccelNed_f(); //   m/s^2
		Data_State_Wind.storage.Zk_V = *stateGetSpeedNed_f(); //    m/s
		Data_State_Wind.storage.Zk_Va =  stateGetAirspeed_f()*cos(stateGetAngleOfAttack_f());    //    m/s  // projection de la norme qur l'axe X (cos sin)
		Data_State_Wind.storage.Zk_AOA = stateGetAngleOfAttack_f();   // rad.
		Data_State_Wind.storage.q = *stateGetNedToBodyQuat_f();
		Data_State_Wind.storage.phi = stateGetNedToBodyEulers_f()->phi; //rad
		Data_State_Wind.storage.theta = stateGetNedToBodyEulers_f()->theta; //rad
		chMtxUnlock(&writeread_state_mtx);
		chSemSignal(&readwrite_state_sem);
	}
}
/*-----------------put_data_to_State------------------*/
/*  Fonction put data in State				 		  */
/*----------------------------------------------------*/
void put_data_to_State(void){
	if(data_to_State==1){

		chMtxLock(&writeread_state_mtx);
		
		NE_Wind_f.x = Answer_State_Wind.storage_tab_float[3];
		NE_Wind_f.y = Answer_State_Wind.storage_tab_float[4];
		
		stateSetAirspeed_f(Answer_State_Wind.storage_tab_float[0]);			//NEED CHECK
		stateSetHorizontalWindspeed_f(&NE_Wind_f);								//NEED CHECK
		stateSetVerticalWindspeed_f(Answer_State_Wind.storage_tab_float[5]);   //NEED CHECK
		
	    chMtxUnlock(&writeread_state_mtx);
		data_to_State=0;
	}
}
/*-------get_wind_from_wind_estimation----------------*/
/* Fonction will get the out's calcul in the structure*/						
/*----------------------------------------------------*/
void get_wind_from_wind_estimation(void){
	int j;
	
	j=0;
	for(int i=0;i<NBR_ANSWER;i++){
		Answer_State_Wind.storage_tab_float[i] = (float)Gen_UKF_Y.xout[j];
		j++;
	}
}
/*-----parse_data_for_wind_estimation-----------------*/
/*  Parse data for the specifique fonction            */	
/*   Before to call the fonction wich will calculate  */						
/*----------------------------------------------------*/
void parse_data_for_wind_estimation(void){
	int i =0;
	int j;
	/*Put data in OmegaA*/
	j=0;
	for(i=0;i<6;i++){
		Gen_UKF_U.omegaa[i]=Data_State_Wind.storage_tab_float[j];
		j++;
	}
	/*Put data in Zk*/
	for(i=0;i<5;i++){
		Gen_UKF_U.zk[i]=Data_State_Wind.storage_tab_float[j];
		j++;
	}
	/*Put data in Q (the quaterion)*/	
	for(i=0;i<4;i++){
		Gen_UKF_U.q[i]=Data_State_Wind.storage_tab_float[j];
		j++;
	}
	/*Put data in Phi*/	
	Gen_UKF_U.phi = Data_State_Wind.storage_tab_float[i];
	i++;
	/*Put data in Alpha*/	
	Gen_UKF_U.theta = Data_State_Wind.storage_tab_float[i];
	
	Gen_UKF_U.dt=time_calculator_dt;
}
/*------------------thd_windestimate------------------*/
/*  Thread fonction                      	 		  */
/*----------------------------------------------------*/
static void thd_windestimate(void *arg)
{
  (void) arg;
  chRegSetThreadName("Start wind estimation");

  while (TRUE){
	  
	  chSemWait (&readwrite_state_sem);
	  
	  chMtxLock(&writeread_state_mtx);
	  
	  if(first_time==1){
		  time_step_before = get_sys_time_msec();
		  time_step_before = 0.02;
		  first_time=0;
	  }else{
		  time_calculator_dt = (get_sys_time_msec()-time_step_before)/1000;
		  time_step_before = get_sys_time_msec();
	  }
	  
	  parse_data_for_wind_estimation();
	  Gen_UKF_step();
	  get_wind_from_wind_estimation();
	  
	  chMtxUnlock(&writeread_state_mtx);
	  data_to_State=1;
  }
}
/*----------------init_calculator---------------------*/
/* Init fonction to init different type of variable   */	
/*  for the calculator before to calculate  		  */					
/*----------------------------------------------------*/
void init_calculator(void){
	
	Gen_UKF_initialize();
		
	Gen_UKF_U.R[0]=pow(1.5,2);
	Gen_UKF_U.R[1]=0;
	Gen_UKF_U.R[2]=0;
	Gen_UKF_U.R[3]=0;
	Gen_UKF_U.R[4]=0;
	
	Gen_UKF_U.R[5]=0;
	Gen_UKF_U.R[6]=pow(1.5,2);
	Gen_UKF_U.R[7]=0;
	Gen_UKF_U.R[8]=0;
	Gen_UKF_U.R[9]=0;
	
	Gen_UKF_U.R[10]=0;
	Gen_UKF_U.R[11]=0;
	Gen_UKF_U.R[12]=pow(1.5,2);
	Gen_UKF_U.R[13]=0;
	Gen_UKF_U.R[14]=0;
	
	Gen_UKF_U.R[15]=0;
	Gen_UKF_U.R[16]=0;
	Gen_UKF_U.R[17]=0;
	Gen_UKF_U.R[18]=pow(0.1,2);
	Gen_UKF_U.R[19]=0;
	
	Gen_UKF_U.R[20]=0;
	Gen_UKF_U.R[21]=0;
	Gen_UKF_U.R[22]=0;
	Gen_UKF_U.R[23]=0;
	Gen_UKF_U.R[2]=pow(0.001,2);
	
	/*-------------------*/
	
	Gen_UKF_U.Q[0]=pow(0.01,2);
	Gen_UKF_U.Q[1]=0;
	Gen_UKF_U.Q[2]=0;
	Gen_UKF_U.Q[3]=0;
	Gen_UKF_U.Q[4]=0;
	Gen_UKF_U.Q[5]=0;
	
	Gen_UKF_U.Q[6]=0;
	Gen_UKF_U.Q[7]=pow(0.01,2);
	Gen_UKF_U.Q[8]=0;
	Gen_UKF_U.Q[9]=0;
	Gen_UKF_U.Q[10]=0;
	Gen_UKF_U.Q[11]=0;
	
	Gen_UKF_U.Q[12]=0;
	Gen_UKF_U.Q[13]=0;
	Gen_UKF_U.Q[14]=pow(0.01,2);
	Gen_UKF_U.Q[15]=0;
	Gen_UKF_U.Q[16]=0;
	Gen_UKF_U.Q[17]=0;
	
	Gen_UKF_U.Q[18]=0;
	Gen_UKF_U.Q[19]=0;
	Gen_UKF_U.Q[20]=0;
	Gen_UKF_U.Q[21]=0.1;
	Gen_UKF_U.Q[22]=0;
	Gen_UKF_U.Q[23]=0;

	Gen_UKF_U.Q[24]=0;
	Gen_UKF_U.Q[25]=0;
	Gen_UKF_U.Q[26]=0;
	Gen_UKF_U.Q[27]=0;
	Gen_UKF_U.Q[28]=0.1;
	Gen_UKF_U.Q[29]=0;
	
	Gen_UKF_U.Q[30]=0;
	Gen_UKF_U.Q[31]=0;
	Gen_UKF_U.Q[32]=0;
	Gen_UKF_U.Q[33]=0;
	Gen_UKF_U.Q[34]=0;
	Gen_UKF_U.Q[35]=0.01;
	
	/*------------------*/
	
	
	Gen_UKF_U.alpha =0.5;
	
	Gen_UKF_U.ki=0;
	
	Gen_UKF_U.beta = 2;
	
	Gen_UKF_U.dt=0.02;
	
	
	/*---------*/
	
	
	Gen_UKF_U.x0[0] = 0;
	Gen_UKF_U.x0[1] = 0;
	Gen_UKF_U.x0[2] = 0;
	Gen_UKF_U.x0[3] = 0;
	Gen_UKF_U.x0[4] = 0;
	Gen_UKF_U.x0[5] = 0;
	
	/*--------------------*/
	
	Gen_UKF_U.P[0]=0.2;
	Gen_UKF_U.P[1]=0;
	Gen_UKF_U.P[2]=0;
	Gen_UKF_U.P[3]=0;
	Gen_UKF_U.P[4]=0;
	Gen_UKF_U.P[5]=0;
	
	Gen_UKF_U.P[6]=0;
	Gen_UKF_U.P[7]=0.2;
	Gen_UKF_U.P[8]=0;
	Gen_UKF_U.P[9]=0;
	Gen_UKF_U.P[10]=0;
	Gen_UKF_U.P[11]=0;
	
	Gen_UKF_U.P[12]=0;
	Gen_UKF_U.P[13]=0;
	Gen_UKF_U.P[14]=0.2;
	Gen_UKF_U.P[15]=0;
	Gen_UKF_U.P[16]=0;
	Gen_UKF_U.P[17]=0;
	
	Gen_UKF_U.P[18]=0;
	Gen_UKF_U.P[19]=0;
	Gen_UKF_U.P[20]=0;
	Gen_UKF_U.P[21]=0.2;
	Gen_UKF_U.P[22]=0;
	Gen_UKF_U.P[23]=0;

	Gen_UKF_U.P[24]=0;
	Gen_UKF_U.P[25]=0;
	Gen_UKF_U.P[26]=0;
	Gen_UKF_U.P[27]=0;
	Gen_UKF_U.P[28]=0.2;
	Gen_UKF_U.P[29]=0;
	
	Gen_UKF_U.P[30]=0;
	Gen_UKF_U.P[31]=0;
	Gen_UKF_U.P[32]=0;
	Gen_UKF_U.P[33]=0;
	Gen_UKF_U.P[34]=0;
	Gen_UKF_U.P[35]=0.2;
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
