#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <Ivy/ivy.h>
#include <Ivy/ivyglibloop.h>
#include "Gen_UKF.h"
#include <inttypes.h>
#include <stdint.h>
#include <time.h>
#include "pprz_algebra_float.h"

#define DEBUG 1

#define NBR_DATA 17     //Number of data to recive

// matrix element
#define MAT_EL(_m, _l, _c, _n) _m[_l + _c * _n]

/*-----Global variables-----*/
int ac_id;                    //Get the id of the ac in the ivy msg

/*----------------------init--------------------------*/
/* Init fonction to init different type of variable   */
/*  for the calculator before to calculate            */
/*----------------------------------------------------*/
void init()
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

/*--------------parse_check_message-------------------*/
/* Fonction call when the message as been recived     */
/*  and parse message to put data in the structure    */
/*----------------------------------------------------*/
static void parse_message(IvyClientPtr app, void *user_data, int argc, char *argv[])
{
  // get AC id
  ac_id = atoi(argv[0]);

  // scan parameters
  int ret = sscanf(argv[1],
      "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
      &rtU.omegaa[0], &rtU.omegaa[1], &rtU.omegaa[2], // p, q, r
      &rtU.omegaa[3], &rtU.omegaa[4], &rtU.omegaa[5], // ax, ay, az
      &rtU.zk[0], &rtU.zk[1], &rtU.zk[2], // vx, vy, vz
      &rtU.zk[3], &rtU.zk[4], // va, aoa
      &rtU.q[0], &rtU.q[1], &rtU.q[2], &rtU.q[3], // quat
      &rtU.phi, &rtU.theta
      );

#if DEBUG
  printf("INPUT %s\n", argv[1]);
#endif

  if (ret != NBR_DATA) {
    printf("Error while parsing data\n");
  }
  else {
    Gen_UKF_step();

    // Send back data over Ivy
    IvySendMsg("Wind_estimator WIND_INFO %d %d %f %f %f %f",
        ac_id, 7,
        rtY.xout[4], // wind east
        rtY.xout[3], // wind north
        -rtY.xout[5], // wind up
        rtY.xout[0] // airspeed
        );

#if DEBUG
    float norm = sqrtf(
        powf(rtY.xout[3], 2) +
        powf(rtY.xout[4], 2) +
        powf(rtY.xout[5], 2)
        );
    printf("WIND_INFO %d %d %f %f %f (%f) %f %f %f\n",
        ac_id, 7,
        rtY.xout[4], // wind east
        rtY.xout[3], // wind north
        -rtY.xout[5], // wind up
        norm, // wind norm
        rtY.xout[0], // airspeed
        rtY.xout[1], // airspeed
        rtY.xout[2] // airspeed
        );
#endif
  }
}

/*--------------------------------------*/
/*          Main fonction               */
/*--------------------------------------*/
int main(int argc, char **argv)
{
  init();

  GMainLoop *ml =  g_main_loop_new(NULL, FALSE);

  IvyInit("Rasp", "Wind Estimate READY", 0, 0, 0, 0);
  IvyStart("127.255.255.255");
  IvyBindMsg(parse_message, 0, "^([0-9]{1,2}) PAYLOAD_FLOAT (.*)");
  g_main_loop_run(ml);

  return 0;
}

