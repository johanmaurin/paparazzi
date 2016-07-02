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
 * @file "modules/wind_raspi_estimator/wind_raspi_estimator.c"
 * @author Maurin
 * Wind Raspberry Pi Estimation
 */
#include "wind_raspi_estimator.h"
#include "state.h"
#include "autopilot.h"
#include "subsystems/datalink/datalink.h"
#include "subsystems/datalink/downlink.h"
#include <math.h>

#include "subsystems/gps.h"
#include "modules/datalink/extra_pprz_dl.h"

union Data_State Data_State;
union Answer_State Answer_State;
float OLD_Data[NBR_DATA];

void get_old_data(void){
	int i;
	 for (i =0;i<NBR_DATA;i++){
		 OLD_Data[i]=Data_State.storage_tab_float[i];
	 }
}

/*------------------get_data_from_state---------------*/
/*  Fonction put data from state to Data_State        */
/*----------------------------------------------------*/
void get_data_from_state(void){
	
	get_old_data();
	
  // get "fake" IMU data in body frame
  struct FloatVect3 accel_ned = {
    stateGetAccelNed_f()->x,
    stateGetAccelNed_f()->y,
    stateGetAccelNed_f()->z
  };
  accel_ned.z -= 9.81f;
  struct FloatRMat *ned_to_body = stateGetNedToBodyRMat_f();
  struct FloatVect3 accel_body;
  float_rmat_vmult(&accel_body, ned_to_body, &accel_ned);

	Data_State.storage.omega = *stateGetBodyRates_f(); //   rad/s
	//Data_State.storage.omega_A = *stateGetAccelNed_f(); //   m/s^2
	Data_State.storage.omega_A.x = accel_body.x; //   m/s^2
	Data_State.storage.omega_A.y = accel_body.y; //   m/s^2
	Data_State.storage.omega_A.z = accel_body.z; //   m/s^2
	Data_State.storage.Zk_V = *stateGetSpeedNed_f(); //    m/s
	Data_State.storage.Zk_Va =  stateGetAirspeed_f();    //    m/s  // projection de la norme qur l'axe X (cos sin)
	//Data_State.storage.Zk_Va =  stateGetAirspeed_f()*cos(stateGetAngleOfAttack_f())*cos(stateGetSideslip_f());    //    m/s  // projection de la norme qur l'axe X (cos sin)
	Data_State.storage.Zk_AOA = stateGetAngleOfAttack_f();   // rad.
	Data_State.storage.q = *stateGetNedToBodyQuat_f();
	Data_State.storage.phi = stateGetNedToBodyEulers_f()->phi; //rad
	Data_State.storage.theta = stateGetNedToBodyEulers_f()->theta; //rad
}
/*--------------storage_data_from_state---------------*/
/*  Use get_data_from_state and check if all data is  */
/*    not at the 0 value							  */
/*   input: void									  */
/*   output: int   									  */
/*				0 if all data to 0 value			  */
/*				1 if not all data to 0 value          */
/*----------------------------------------------------*/
int storage_data_from_state(){
	
	int  j=0;
	get_data_from_state();
	
	for(int i= 0; i<NBR_DATA;i++){
		if(Data_State.storage_tab_float[i]==OLD_Data[i]){
			j++;
		}
	}
	
	/*if(j==NBR_DATA){
		return 0; //Not for test
	}*/
	return 1;
}
/*--------------get_wind_from_wind_estimation---------*/
/*  		    									  */
/*----------------------------------------------------*/
void get_wind_from_wind_estimation(){
	
	int i=0;
	const char s[2] = " ";
	char *token;
   
	token = strtok((char*)dl_buffer, s);
	while( (token != NULL) && (i<NBR_DATA)) 
	{
		Answer_State.storage_tab_float[i]=(float)atof(token);	
		token = strtok(NULL, s);
		i++;
	}
	if(i==NBR_ANSWER){
	}
}
/*--------------init_extra_pprz_raspi-----------------*/
/*  Init fonction 									  */
/*----------------------------------------------------*/
void init_extra_pprz_raspi(void)
{
	extra_pprz_dl_init();
}
/*--------------send_data_to_wind_estimation----------*/
/*  Call at 50Hz send data from state to the wind     */
/*    				estimator with PAYLOAD_FLOAT      */
/*----------------------------------------------------*/
void send_data_to_wind_estimation(void)
{
	if(storage_data_from_state()){
		DOWNLINK_SEND_PAYLOAD_FLOAT(extra_pprz_tp, EXTRA_DOWNLINK_DEVICE ,
                     NBR_DATA,  Data_State.storage_tab_float);
	}
}
