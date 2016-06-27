#ifndef UKF_LIB
#define UKF_LIB
#include <stdio.h>
#include <stdlib.h>
#include "../../../math/pprz_algebra_float.h"
#include "../../../math/pprz_geodetic_float.h"
#include "Gen_UKF.h"
/* Different variable about the data size*/
#define NBR_DATA 17		  //Number of data to recive
#define NBR_ANSWER 6	  //Number of data to answer
/*Structur of the data to storage before to calcul*/
struct Data_struc{
	struct FloatRates omega;
/*	float  omegaa_p;
	float  omegaa_q;
	float  omegaa_r;*/
	struct EcefCoor_f omega_A;
/*	float  omegaa_Ax;
	float  omegaa_Ay;
	float  omegaa_Az;*/
	struct NedCoor_f Zk_V;
/*	float  Zk_vx;
	float  Zk_vy;
	float  Zk_vz;*/
	float  Zk_Va;
	float  Zk_AOA;
	struct FloatQuat q;
/*	float  q1;
	float  q2;
	float  q3;
	float  q4;*/
	float  phi;
	float  theta;
};
/*--------------Union to convert the Structure to a float array----------*/
union Data_State{
	struct Data_struc storage;
	float storage_tab_float[NBR_DATA];
};
extern union Data_State Data_State;
/*Structur of the data to recive from the calcul*/
struct Answer_storage{
  float xout_1;
  float xout_2;
  float xout_3;
  float xout_4;
  float xout_5;
  float xout_6;                   
};
/*Union to generate a array of float link to the data to send*/
union Answer_State{
	struct Answer_storage storage;
	float storage_tab_float[NBR_ANSWER];
};
extern union Answer_State Answer_State;

extern void parse_data_for_wind_estimation(void);
extern void get_wind(void);
extern void init_calculator(void);

#endif
