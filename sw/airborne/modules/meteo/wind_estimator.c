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
#include "mcu_periph/sys_time.h"
#include "modules/loggers/sdlog_chibios/sdLog.h"
#include "modules/loggers/sdlog_chibios/usbStorage.h"
#include "modules/loggers/sdlog_chibios.h"
#include "wind_estimator.h"
#include "lib_ukf_wind_estimator/Gen_UKF.h"

// matrix element
#define MAT_EL(_m, _l, _c, _n) _m[_l + _c * _n]

/*--------------------Varibales---------------------*/
static MUTEX_DECL(writeread_state_mtx);
static SEMAPHORE_DECL(readwrite_state_sem, 0);

int data_to_State = 0;          //Flag for event to write new data in State
int first_time = 1;

int time_step_before;
int time_calculator_dt;

struct FloatVect2 NE_Wind_f;

/* Data use by the calculator */
union Data_State_Wind Data_State_Wind;
union Answer_State_Wind Answer_State_Wind;

/*----------------------------------------------------*/
/*
 * Start wind estimation thread
 */
static THD_WORKING_AREA(wa_thd_windestimation, 6 * 1024);
static __attribute__((noreturn)) void thd_windestimate(void *arg);

/*----------------get_data_from_state-----------------*/
/*  Put Data from State in struct use by the Thread   */
/*----------------------------------------------------*/
void get_data_from_state(void)
{
  if (chMtxTryLock(&writeread_state_mtx)) {
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

    Data_State_Wind.storage.omega = *stateGetBodyRates_f(); //   rad/s
    //Data_State_Wind.storage.omega_A = *stateGetAccelNed_f(); //   m/s^2
    Data_State_Wind.storage.omega_A.x = accel_body.x; //   m/s^2
    Data_State_Wind.storage.omega_A.y = accel_body.y; //   m/s^2
    Data_State_Wind.storage.omega_A.z = accel_body.z; //   m/s^2
    Data_State_Wind.storage.Zk_V = *stateGetSpeedNed_f(); //    m/s
    //Data_State_Wind.storage.Zk_Va =  stateGetAirspeed_f() * cosf(stateGetAngleOfAttack_f());  //    m/s  // projection de la norme qur l'axe X (cos sin)
    Data_State_Wind.storage.Zk_Va =  stateGetAirspeed_f();  //    m/s  // projection de la norme qur l'axe X (cos sin)
    Data_State_Wind.storage.Zk_AOA = stateGetAngleOfAttack_f();   // rad.
    Data_State_Wind.storage.q = *stateGetNedToBodyQuat_f();
    Data_State_Wind.storage.phi = stateGetNedToBodyEulers_f()->phi; //rad
    Data_State_Wind.storage.theta = stateGetNedToBodyEulers_f()->theta; //rad
    chMtxUnlock(&writeread_state_mtx);
    chSemSignal(&readwrite_state_sem);
  }
}

/*-----------------put_data_to_State------------------*/
/*  Fonction put data in State                        */
/*----------------------------------------------------*/
void put_data_to_State(void)
{
  if (data_to_State == 1) {

    chMtxLock(&writeread_state_mtx);

    NE_Wind_f.x = Answer_State_Wind.storage_tab_float[3];
    NE_Wind_f.y = Answer_State_Wind.storage_tab_float[4];

    stateSetAirspeed_f(Answer_State_Wind.storage_tab_float[0]);     //NEED CHECK
    stateSetHorizontalWindspeed_f(&NE_Wind_f);                //NEED CHECK
    stateSetVerticalWindspeed_f(Answer_State_Wind.storage_tab_float[5]);   //NEED CHECK

    chMtxUnlock(&writeread_state_mtx);
    data_to_State = 0;
  }
}

/*-------get_wind_from_wind_estimation----------------*/
/* Fonction will get the out's calcul in the structure*/
/*----------------------------------------------------*/
void get_wind_from_wind_estimation(void)
{
  int j;

  j = 0;
  for (int i = 0; i < NBR_ANSWER; i++) {
    Answer_State_Wind.storage_tab_float[i] = (float)rtY.xout[j];
    j++;
  }
}

/*-----parse_data_for_wind_estimation-----------------*/
/*  Parse data for the specifique fonction            */
/*   Before to call the fonction wich will calculate  */
/*----------------------------------------------------*/
void parse_data_for_wind_estimation(void)
{
  int i = 0;
  int j;
  /*Put data in OmegaA*/
  j = 0;
  for (i = 0; i < 6; i++) {
    rtU.omegaa[i] = Data_State_Wind.storage_tab_float[j];
    j++;
  }
  /*Put data in Zk*/
  for (i = 0; i < 5; i++) {
    rtU.zk[i] = Data_State_Wind.storage_tab_float[j];
    j++;
  }
  /*Put data in Q (the quaterion)*/
  for (i = 0; i < 4; i++) {
    rtU.q[i] = Data_State_Wind.storage_tab_float[j];
    j++;
  }
  /*Put data in Phi*/
  rtU.phi = Data_State_Wind.storage_tab_float[i];
  i++;
  /*Put data in Alpha*/
  rtU.theta = Data_State_Wind.storage_tab_float[i];

  rtU.dt = time_calculator_dt;
}

/*------------------thd_windestimate------------------*/
/*  Thread fonction                                   */
/*----------------------------------------------------*/
static void thd_windestimate(void *arg)
{
  (void) arg;
  chRegSetThreadName("Start wind estimation");

  while (TRUE) {

    chSemWait(&readwrite_state_sem);

    chMtxLock(&writeread_state_mtx);

    if (first_time == 1) {
      time_step_before = get_sys_time_msec();
      time_step_before = 0.02;
      first_time = 0;
    } else {
      time_calculator_dt = (get_sys_time_msec() - time_step_before) / 1000;
      time_step_before = get_sys_time_msec();
    }

    parse_data_for_wind_estimation();
    Gen_UKF_step();
    get_wind_from_wind_estimation();

    chMtxUnlock(&writeread_state_mtx);
    data_to_State = 1;
  }
}

/*----------------init_calculator---------------------*/
/* Init fonction to init different type of variable   */
/*  for the calculator before to calculate            */
/*----------------------------------------------------*/
void init_calculator(void)
{

  Gen_UKF_initialize();

  memset(rtU.x0, 0, 6 * sizeof(float));

  memset(rtU.P_i, 0, 36 * sizeof(float));
  MAT_EL(rtU.P_i, 0, 0, 6) = 0.2;
  MAT_EL(rtU.P_i, 1, 1, 6) = 0.2;
  MAT_EL(rtU.P_i, 2, 2, 6) = 0.2;
  MAT_EL(rtU.P_i, 3, 3, 6) = 0.2;
  MAT_EL(rtU.P_i, 4, 4, 6) = 0.2;
  MAT_EL(rtU.P_i, 5, 5, 6) = 0.2;

  memset(rtU.R, 0, 25 * sizeof(float));
  MAT_EL(rtU.R, 0, 0, 5) = powf(0.5, 2);
  MAT_EL(rtU.R, 1, 1, 5) = powf(0.5, 2);
  MAT_EL(rtU.R, 2, 2, 5) = powf(0.5, 2);
  MAT_EL(rtU.R, 3, 3, 5) = powf(0.01, 2);
  MAT_EL(rtU.R, 4, 4, 5) = powf(0.001, 2);

  memset(rtU.Q, 0, 36 * sizeof(float));
  MAT_EL(rtU.Q, 0, 0, 6) = powf(1, 2);
  MAT_EL(rtU.Q, 1, 1, 6) = powf(1, 2);
  MAT_EL(rtU.Q, 2, 2, 6) = powf(1, 2);
  MAT_EL(rtU.Q, 3, 3, 6) = powf(0.1, 2);
  MAT_EL(rtU.Q, 4, 4, 6) = powf(0.1, 2);
  MAT_EL(rtU.Q, 5, 5, 6) = powf(0.1, 2);

  rtU.ki = 0.;
  rtU.alpha = 0.5;
  rtU.beta = 2;
  rtU.dt = 0.1; // FIXME measure time

}
/*-----------------wind_estimator_init----------------*/
/*  Init the Thread and the calculator          */
/*----------------------------------------------------*/
void wind_estimator_init(void)
{
  init_calculator();

  // Start wind estimation thread
  chThdCreateStatic(wa_thd_windestimation, sizeof(wa_thd_windestimation),
                    NORMALPRIO + 2, thd_windestimate, NULL);
}
