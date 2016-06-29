#ifndef RASPB
#define RASPB
#include <stdio.h>
#include <stdlib.h>
/* Different variable about the data size*/
#define NBR_DIFF_DATA 5   //Number of the different type of data to recive
#define NBR_DATA 17		  //Number of data to recive
#define NBR_DIFF_ANSWER 2 //Number of the different type of data to answer
#define NBR_ANSWER 42	  //Number of data to answer
#define NBR_DATA_SEND_BACK 6
#define NAME_FILE "Log_file_data_"

/*-----------------------name_data_receive-------------------------*/
/*  Different name of input  data from the calculator              */
/*-----------------------------------------------------------------*/
char* name_data_receive[NBR_DATA]={
	"omegaa_p ",
	"omegaa_q ",
	"omegaa_r ",
	"omegaa_Ax",
	"omegaa_Ay",
	"omegaa_Az",
	"Zk_vx    ",
	"Zk_vy    ",
	"Zk_vz    ",
	"Zk_Va    ",
	"Zk_AOA   ",
	"q1       ",
	"q2       ",
	"q3       ",
	"q4       ",
	"phi      ",
	"theta    "
};
/*-----------------------name_data_send_back-----------------------*/
/*  Different name of output data from the calculator              */
/*-----------------------------------------------------------------*/
char* name_data_send_back[NBR_DATA_SEND_BACK]={
	"Airspeed",
	"Vk      ",
	"Wk      ",
	"East    ",
	"North   ",
	"Down    "
};
/*-----------------------size_of_answer----------------------------*/
/*Array of int to precise the size of the different type of data   */
/* (ther place in the chaine of data than the program will send)   */
/*-----------------------------------------------------------------*/
int size_of_answer[NBR_DIFF_ANSWER]={
2,
36
};
/*Structur of the data to storage before to calcul*/
struct Data_storage {
	float  omegaa_p;
	float  omegaa_q;
	float  omegaa_r;
	float  omegaa_Ax;
	float  omegaa_Ay;
	float  omegaa_Az;
	float  Zk_vx;
	float  Zk_vy;
	float  Zk_vz;
	float  Zk_Va;
	float  Zk_AOA;
	float  q1;
	float  q2;
	float  q3;
	float  q4;
	float  phi;
	float  theta;
};
/*Union to generate a array of float link to the data to recive*/
union Data_State{
	struct Data_storage storage;
	float storage_tab_float[NBR_DATA];
};
union Data_State Data_State;
/*Structur of the data to recive from the calcul*/
struct Answer_storage{
  float xout_1;
  float xout_2;
  float xout_3;
  float xout_4;
  float xout_5;
  float xout_6;
                        
  float Pout_1;
  float Pout_2; 
  float Pout_3; 
  float Pout_4; 
  float Pout_5; 
  float Pout_6; 
  
  float Pout_7; 
  float Pout_8; 
  float Pout_9; 
  float Pout_10; 
  float Pout_11; 
  float Pout_12; 
  
  float Pout_13; 
  float Pout_14; 
  float Pout_15; 
  float Pout_16; 
  float Pout_17; 
  float Pout_18; 
  
  float Pout_19; 
  float Pout_20; 
  float Pout_21; 
  float Pout_22; 
  float Pout_23; 
  float Pout_24; 
  
  float Pout_25; 
  float Pout_26; 
  float Pout_27; 
  float Pout_28; 
  float Pout_29; 
  float Pout_30; 
  
  float Pout_31; 
  float Pout_32; 
  float Pout_33; 
  float Pout_34; 
  float Pout_35; 
  float Pout_36;                     
};
/*Union to generate a array of float link to the data to send*/
union Answer_State{
	struct Answer_storage storage;
	float storage_tab_float[NBR_ANSWER];
};
union Answer_State Answer_State;

void send_wind_estimation();
void send_error(); //empty
void parse_data_for_wind_estimation();
void get_wind();
void init();

#endif
