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
		Answer_State_Wind.storage_tab_float[i] = (float)ExtY.xout[j];
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
		ExtU.omegaa[i]=Data_State_Wind.storage_tab_float[j];
		j++;
	}
	/*Put data in Zk*/
	for(i=0;i<5;i++){
		ExtU.zk[i]=Data_State_Wind.storage_tab_float[j];
		j++;
	}
	/*Put data in Q (the quaterion)*/	
	for(i=0;i<4;i++){
		ExtU.q[i]=Data_State_Wind.storage_tab_float[j];
		j++;
	}
	/*Put data in Phi*/	
	ExtU.phi = Data_State_Wind.storage_tab_float[i];
	i++;
	/*Put data in Alpha*/	
	ExtU.theta = Data_State_Wind.storage_tab_float[i];
	
	ExtU.dt=time_calculator_dt;
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
		
	ExtU.R[0]=pow(1.5,2);
	ExtU.R[1]=0;
	ExtU.R[2]=0;
	ExtU.R[3]=0;
	ExtU.R[4]=0;
	
	ExtU.R[5]=0;
	ExtU.R[6]=pow(1.5,2);
	ExtU.R[7]=0;
	ExtU.R[8]=0;
	ExtU.R[9]=0;
	
	ExtU.R[10]=0;
	ExtU.R[11]=0;
	ExtU.R[12]=pow(1.5,2);
	ExtU.R[13]=0;
	ExtU.R[14]=0;
	
	ExtU.R[15]=0;
	ExtU.R[16]=0;
	ExtU.R[17]=0;
	ExtU.R[18]=pow(0.1,2);
	ExtU.R[19]=0;
	
	ExtU.R[20]=0;
	ExtU.R[21]=0;
	ExtU.R[22]=0;
	ExtU.R[23]=0;
	ExtU.R[2]=pow(0.001,2);
	
	/*-------------------*/
	
	ExtU.Q[0]=pow(0.01,2);
	ExtU.Q[1]=0;
	ExtU.Q[2]=0;
	ExtU.Q[3]=0;
	ExtU.Q[4]=0;
	ExtU.Q[5]=0;
	
	ExtU.Q[6]=0;
	ExtU.Q[7]=pow(0.01,2);
	ExtU.Q[8]=0;
	ExtU.Q[9]=0;
	ExtU.Q[10]=0;
	ExtU.Q[11]=0;
	
	ExtU.Q[12]=0;
	ExtU.Q[13]=0;
	ExtU.Q[14]=pow(0.01,2);
	ExtU.Q[15]=0;
	ExtU.Q[16]=0;
	ExtU.Q[17]=0;
	
	ExtU.Q[18]=0;
	ExtU.Q[19]=0;
	ExtU.Q[20]=0;
	ExtU.Q[21]=0.1;
	ExtU.Q[22]=0;
	ExtU.Q[23]=0;

	ExtU.Q[24]=0;
	ExtU.Q[25]=0;
	ExtU.Q[26]=0;
	ExtU.Q[27]=0;
	ExtU.Q[28]=0.1;
	ExtU.Q[29]=0;
	
	ExtU.Q[30]=0;
	ExtU.Q[31]=0;
	ExtU.Q[32]=0;
	ExtU.Q[33]=0;
	ExtU.Q[34]=0;
	ExtU.Q[35]=0.01;
	
	/*------------------*/
	
	
	ExtU.alpha =0.5;
	
	ExtU.ki=0;
	
	ExtU.beta = 2;
	
	ExtU.dt=0.02;
	
	
	/*---------*/
	
	
	ExtU.x0[0] = 0;
	ExtU.x0[1] = 0;
	ExtU.x0[2] = 0;
	ExtU.x0[3] = 0;
	ExtU.x0[4] = 0;
	ExtU.x0[5] = 0;
	
	/*--------------------*/
	
	ExtU.P_i[0]=0.2;
	ExtU.P_i[1]=0;
	ExtU.P_i[2]=0;
	ExtU.P_i[3]=0;
	ExtU.P_i[4]=0;
	ExtU.P_i[5]=0;
	
	ExtU.P_i[6]=0;
	ExtU.P_i[7]=0.2;
	ExtU.P_i[8]=0;
	ExtU.P_i[9]=0;
	ExtU.P_i[10]=0;
	ExtU.P_i[11]=0;
	
	ExtU.P_i[12]=0;
	ExtU.P_i[13]=0;
	ExtU.P_i[14]=0.2;
	ExtU.P_i[15]=0;
	ExtU.P_i[16]=0;
	ExtU.P_i[17]=0;
	
	ExtU.P_i[18]=0;
	ExtU.P_i[19]=0;
	ExtU.P_i[20]=0;
	ExtU.P_i[21]=0.2;
	ExtU.P_i[22]=0;
	ExtU.P_i[23]=0;

	ExtU.P_i[24]=0;
	ExtU.P_i[25]=0;
	ExtU.P_i[26]=0;
	ExtU.P_i[27]=0;
	ExtU.P_i[28]=0.2;
	ExtU.P_i[29]=0;
	
	ExtU.P_i[30]=0;
	ExtU.P_i[31]=0;
	ExtU.P_i[32]=0;
	ExtU.P_i[33]=0;
	ExtU.P_i[34]=0;
	ExtU.P_i[35]=0.2;
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
