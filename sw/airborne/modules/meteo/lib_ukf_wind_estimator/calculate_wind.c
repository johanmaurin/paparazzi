#include <stdio.h>
#include <stdlib.h>
#include "calculate_wind.h"
#include <inttypes.h>
#include <stdint.h>

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
	Gen_UKF_step();
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
