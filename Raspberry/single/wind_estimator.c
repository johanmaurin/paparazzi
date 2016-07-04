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
#define NAME_FILE "Log_file_data_"
#define NBR_DATA 17     //Number of data to recive

// matrix element
#define MAT_EL(_m, _l, _c, _n) _m[_l + _c * _n]

/*-----Global variables-----*/
int ac_id;                    //Get the id of the ac in the ivy msg
int print_debug_opt=0;
int log_debug_opt=0;
int file_name_default=1;
char name_file[10];
FILE * file_logger =NULL;

void gen_string_for_file(char* string,int size){
	int i=0;
	const char first[3]="%f,";
	while(i<size-1){
		strcat(string,first);
		i++;
	}
	strcat(string,"%f\n");
}

void log_file_init_var_of_Gen_UKF(){
	char string_for_log_file[300];
	
	strcpy(string_for_log_file, "#P_i: ");
	gen_string_for_file(string_for_log_file,36);
	
	fprintf(file_logger, string_for_log_file, 
	MAT_EL(rtU.P_i, 0, 0, 6), MAT_EL(rtU.P_i, 0, 1, 6), MAT_EL(rtU.P_i, 0, 2, 6), MAT_EL(rtU.P_i, 0, 3, 6), MAT_EL(rtU.P_i, 0, 4, 6), MAT_EL(rtU.P_i, 0, 5, 6),
	MAT_EL(rtU.P_i, 1, 0, 6), MAT_EL(rtU.P_i, 1, 1, 6), MAT_EL(rtU.P_i, 1, 2, 6), MAT_EL(rtU.P_i, 1, 3, 6), MAT_EL(rtU.P_i, 1, 4, 6), MAT_EL(rtU.P_i, 1, 5, 6),
	MAT_EL(rtU.P_i, 2, 0, 6), MAT_EL(rtU.P_i, 2, 1, 6), MAT_EL(rtU.P_i, 2, 2, 6), MAT_EL(rtU.P_i, 2, 3, 6), MAT_EL(rtU.P_i, 2, 4, 6), MAT_EL(rtU.P_i, 2, 5, 6),
	MAT_EL(rtU.P_i, 3, 0, 6), MAT_EL(rtU.P_i, 3, 1, 6), MAT_EL(rtU.P_i, 3, 2, 6), MAT_EL(rtU.P_i, 3, 3, 6), MAT_EL(rtU.P_i, 3, 4, 6), MAT_EL(rtU.P_i, 3, 5, 6),
	MAT_EL(rtU.P_i, 4, 0, 6), MAT_EL(rtU.P_i, 4, 1, 6), MAT_EL(rtU.P_i, 4, 2, 6), MAT_EL(rtU.P_i, 4, 3, 6), MAT_EL(rtU.P_i, 4, 4, 6), MAT_EL(rtU.P_i, 4, 5, 6),
	MAT_EL(rtU.P_i, 5, 0, 6), MAT_EL(rtU.P_i, 5, 1, 6), MAT_EL(rtU.P_i, 5, 2, 6), MAT_EL(rtU.P_i, 5, 3, 6), MAT_EL(rtU.P_i, 5, 4, 6), MAT_EL(rtU.P_i, 5, 5, 6)
	);
	
	strcpy(string_for_log_file, "#R: ");
	gen_string_for_file(string_for_log_file,25);
	fprintf(file_logger, string_for_log_file, 
	MAT_EL(rtU.R, 0, 0, 5),MAT_EL(rtU.R, 0, 1, 5),MAT_EL(rtU.R, 0, 2, 5),MAT_EL(rtU.R, 0, 3, 5),MAT_EL(rtU.R, 0, 4, 5),
	MAT_EL(rtU.R, 1, 0, 5),MAT_EL(rtU.R, 1, 1, 5),MAT_EL(rtU.R, 1, 2, 5),MAT_EL(rtU.R, 1, 3, 5),MAT_EL(rtU.R, 1, 4, 5),
	MAT_EL(rtU.R, 2, 0, 5),MAT_EL(rtU.R, 2, 1, 5),MAT_EL(rtU.R, 2, 2, 5),MAT_EL(rtU.R, 2, 3, 5),MAT_EL(rtU.R, 2, 4, 5),
	MAT_EL(rtU.R, 3, 0, 5),MAT_EL(rtU.R, 3, 1, 5),MAT_EL(rtU.R, 3, 2, 5),MAT_EL(rtU.R, 3, 3, 5),MAT_EL(rtU.R, 3, 4, 5),
	MAT_EL(rtU.R, 4, 0, 5),MAT_EL(rtU.R, 4, 1, 5),MAT_EL(rtU.R, 4, 2, 5),MAT_EL(rtU.R, 4, 3, 5),MAT_EL(rtU.R, 4, 4, 5)
	);
	
	strcpy(string_for_log_file, "#Q: ");
	gen_string_for_file(string_for_log_file,36);
	
	fprintf(file_logger, string_for_log_file, 
	MAT_EL(rtU.Q, 0, 0, 6), MAT_EL(rtU.Q, 0, 1, 6), MAT_EL(rtU.Q, 0, 2, 6), MAT_EL(rtU.Q, 0, 3, 6), MAT_EL(rtU.Q, 0, 4, 6), MAT_EL(rtU.Q, 0, 5, 6),
	MAT_EL(rtU.Q, 1, 0, 6), MAT_EL(rtU.Q, 1, 1, 6), MAT_EL(rtU.Q, 1, 2, 6), MAT_EL(rtU.Q, 1, 3, 6), MAT_EL(rtU.Q, 1, 4, 6), MAT_EL(rtU.Q, 1, 5, 6),
	MAT_EL(rtU.Q, 2, 0, 6), MAT_EL(rtU.Q, 2, 1, 6), MAT_EL(rtU.Q, 2, 2, 6), MAT_EL(rtU.Q, 2, 3, 6), MAT_EL(rtU.Q, 2, 4, 6), MAT_EL(rtU.Q, 2, 5, 6),
	MAT_EL(rtU.Q, 3, 0, 6), MAT_EL(rtU.Q, 3, 1, 6), MAT_EL(rtU.Q, 3, 2, 6), MAT_EL(rtU.Q, 3, 3, 6), MAT_EL(rtU.Q, 3, 4, 6), MAT_EL(rtU.Q, 3, 5, 6),
	MAT_EL(rtU.Q, 4, 0, 6), MAT_EL(rtU.Q, 4, 1, 6), MAT_EL(rtU.Q, 4, 2, 6), MAT_EL(rtU.Q, 4, 3, 6), MAT_EL(rtU.Q, 4, 4, 6), MAT_EL(rtU.Q, 4, 5, 6),
	MAT_EL(rtU.Q, 5, 0, 6), MAT_EL(rtU.Q, 5, 1, 6), MAT_EL(rtU.Q, 5, 2, 6), MAT_EL(rtU.Q, 5, 3, 6), MAT_EL(rtU.Q, 5, 4, 6), MAT_EL(rtU.Q, 5, 5, 6)
	);
	
	fprintf(file_logger, "#ki:%f\n",rtU.ki);
	
	fprintf(file_logger, "#alpha:%f\n",rtU.alpha);
	
	fprintf(file_logger, "#beta:%f\n",rtU.beta);
	
	fprintf(file_logger, "#dt:%f\n",rtU.dt);
	
	fprintf(file_logger, "p,q,r,ax,ay,az,vx,vy,vz,va,aoa,q1,q2,q3,q4,phi,theta,uw0,vw0,ww0,norm,uk,vk,wk\n");
	
	fflush(file_logger);
}

void open_file_log(){
	
  uint32_t counter = 0;
  char filename[512];
  time_t rawtime;
  struct tm * timeinfo;
  
  if(file_name_default){
	  // Check for available files
	  sprintf(filename, "%s%d", NAME_FILE, counter);
	  while ((file_logger = fopen(filename, "r"))) {
		fclose(file_logger);
		counter++;
		sprintf(filename, "%s%d", NAME_FILE, counter);
	  }
  }else{
	  strcpy(filename,name_file);
  }
  
  file_logger = fopen(filename, "w+");
  if (file_logger != NULL) {
	  time ( &rawtime );
      timeinfo = localtime ( &rawtime );
	  fprintf(file_logger, "Log_file start at : %s\n", asctime(timeinfo));
	  log_file_init_var_of_Gen_UKF();
  }
}
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

  if(log_debug_opt){
	  open_file_log();
  }
}

/*--------------parse_check_message-------------------*/
/* Fonction call when the message as been recived     */
/*  and parse message to put data in the structure    */
/*----------------------------------------------------*/
static void parse_message(IvyClientPtr app, void *user_data, int argc, char *argv[])
{
  char string_for_log_file[150];
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

  if(print_debug_opt){
	  printf("INPUT %s\n", argv[1]);
  }

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
        
	float norm = sqrtf(
			powf(rtY.xout[3], 2) +
			powf(rtY.xout[4], 2) +
			powf(rtY.xout[5], 2)
			);
			
    if(print_debug_opt){
		
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
	}
	if(log_debug_opt){
		strcpy(string_for_log_file,"%s,");
		gen_string_for_file(string_for_log_file,7);
		fprintf(file_logger, string_for_log_file,
		argv[1],
        rtY.xout[4], // wind east
		rtY.xout[3], // wind north
		-rtY.xout[5], // wind up
		norm, // wind norm
		rtY.xout[0], // airspeed
		rtY.xout[1], // airspeed
		rtY.xout[2] // airspeed
		);
		fflush(file_logger);
	}
  }
}

void option(char **argv){
	int i =1;
    while(argv[i] && i<=3){
		if(strcmp(argv[i], "-l")==0){
			log_debug_opt=1;
		}
		else{
			if(strcmp(argv[i], "-v")==0){
			print_debug_opt=1;
			}
			else{
				if(strlen(argv[i])>3){
					printf("Get name of file\n");
					strcpy(name_file,argv[i]);
					file_name_default=0;
				}
			}
		}
		i++;
	}
}
/*--------------------------------------*/
/*          Main fonction               */
/*--------------------------------------*/
int main(int argc, char **argv)
{
  option(argv);
  init();
	
  GMainLoop *ml =  g_main_loop_new(NULL, FALSE);
  
  IvyInit("Rasp", "Wind Estimate READY", 0, 0, 0, 0);
  IvyStart("127.255.255.255");
  IvyBindMsg(parse_message, 0, "^([0-9]{1,2}) PAYLOAD_FLOAT (.*)");
  g_main_loop_run(ml);

  return 0;
}

