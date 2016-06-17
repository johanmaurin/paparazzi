#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include "raspberry.h"
#include <Ivy/ivy.h>
#include <Ivy/ivyglibloop.h>
#include "Gen_UKF.h"
#include <inttypes.h>
#include <stdint.h>
/*--------------------------------------Global variables---------------------------------------*/
#define VAL_DEFAULT -100000.42					//Default valeur for old data
	
int ID_AC = 1;									//ID of the aircraft
float old_data_2[NBR_DATA_SEND_BACK-2];			//Stock data to compare
int i_2=0;										//location in the tabl of old data
int compare = 0;								//Number of same data betwenn old and new data
FILE * fp;
/*---------------------------------------------------------------------------------------------*/
/*--------------------init_old------------------------*/
/* Init the old data at begin with   VAL_DEFAULT      */
/*----------------------------------------------------*/
void init_old(){
	int i;
	for(i=0;i<(NBR_DATA_SEND_BACK-2);i++){
		old_data_2[i]=VAL_DEFAULT;
	}
}
/*------------------reverse_string--------------------*/
/*  Reverse the char* but not char by char but        */
/*    chaine of char separate by " "  (space)         */
/*          full_data: char* to reverse      		  */
/*----------------------------------------------------*/
void reverse_string(char* full_data){
	
	char new[NBR_DATA_SEND_BACK][10];
	
	int i=0;
	const char s[2] = " ";
	char *token;
   
	token = strtok(full_data, s);
	while( (token != NULL) && (i<NBR_DATA_SEND_BACK)) 
	{
		strcpy(new[i], token);
		token = strtok(NULL, s);
		i++;
	}
	strcpy(full_data,new[NBR_DATA_SEND_BACK-1]);
	for(i=NBR_DATA_SEND_BACK-2;i>-1;i--){
		strcat(full_data, " ");
		strcat(full_data,new[i]);
	}
	free(token);
}
/*---------add_data_to_ivy_string_UINT8---------------*/
/*  Add a data to the char* full_data                 */
/*    char* full_data: char* where the var will be add*/
/*    int   var      : data to add 					  */
/*    int   first    :  1 if the first data else 0    */
/*----------------------------------------------------*/
void add_data_to_ivy_string_UINT8( char * full_data, int var,int first){
	
	char array[10];
	sprintf(array, "%d",(uint8_t)var);
	strcat(array, " ");
    if(first==1){
		strcpy(full_data, array);
	}else{
		strcat(full_data, array);
	}
    if(old_data_2[i_2]!= VAL_DEFAULT){
		if(var==(int)old_data_2[i_2]){
			compare++;
		}
	}
    old_data_2[i_2]=(float)var;
    i_2++;
    
}
/*---------add_data_to_ivy_string_FLOAT---------------*/
/*  Add a data to the char* full_data                 */
/*    char* full_data: char* where the var will be add*/
/*    flaot var      : data to add 					  */
/*    int   first    :  1 if the first data else 0    */
/*----------------------------------------------------*/
void add_data_to_ivy_string_FLOAT( char * full_data, float var,int first){
	
	char array[10];
	sprintf(array, "%f",(float)var);
	strcat(array, " ");
	if(first==1){
		strcpy(full_data, array);
	}else{
		strcat(full_data, array);
	}
    
    if(old_data_2[i_2]!= VAL_DEFAULT){
		if(var==(float)old_data_2[i_2]){
			compare++;
		}
	}
    old_data_2[i_2]=(float)var;
    i_2++;
}
/*--------------send_wind_estimation------------------*/
/*  Fonction call to send data to the airdraft        */
/*    This fonction change the float array to a chaine*/
/*          of char									  */
/*----------------------------------------------------*/
void send_wind_estimation(){
	          
	int i; 
	get_wind();  
	char array[10];                            //array will containe the different float value
    char *full_data = (char *) malloc(sizeof(array)*NBR_DATA_SEND_BACK); //Chaine of char will containe the all data
    
    /*The last data*/
	/*Down : Xout 6*/
	add_data_to_ivy_string_FLOAT(full_data,Answer_State.storage_tab_float[5],1);
	/*The Forth data is a copy*/ 
	/*North : Xout 3*/
    add_data_to_ivy_string_FLOAT(full_data,Answer_State.storage_tab_float[3],0);
    /*The Third data is a copy*/ 
    /*Eath : Xout 4*/
	add_data_to_ivy_string_FLOAT(full_data,Answer_State.storage_tab_float[4],0);
	/*The Second data is a copy*/ 
   add_data_to_ivy_string_UINT8(full_data,compare,0);
	/*The First data is a copy*/ 
	add_data_to_ivy_string_UINT8(full_data,ID_AC,0);
	/*reverse the string of data (full_data)*/
	reverse_string(full_data);
	/*Send data to ivy*/
	IvySendMsg("Wind_estimator WIND_INFO %s", full_data);
	/*reset some param for the new step*/
    i_2=0;
    compare = 0;
    printf("\nDATA\n");
    fprintf(fp, "DATA_GEN_UKF:\n");
    for(i =0;i<6;i++){
		printf("%f\n",Answer_State.storage_tab_float[i]);
		fprintf(fp, "%f\n", Answer_State.storage_tab_float[i]);
	}
    fflush(fp);
    free(full_data);
}
/*--------------send_error----------------------------*/
/*  Fonction for fellback error if needed             */							
/*----------------------------------------------------*/
void send_error(){
	int i=0;
	printf("Small one\n");
	while(i<NBR_DATA) 
	{
		printf( "%f\n",Data_State.storage_tab_float[i]);	
		i++;
	}
}
/*-----parse_data_for_wind_estimation-----------------*/
/*  Parse data for the specifique fonction            */	
/*   Before to call the fonction wich will calculate  */						
/*----------------------------------------------------*/
void parse_data_for_wind_estimation(){
	int i =0;
	int j;
	/*Put data in OmegaA*/
	j=0;
	for(i=0;i<6;i++){
		Gen_UKF_U.omegaa[i]=Data_State.storage_tab_float[j];
		j++;
	}
	/*Put data in Zk*/
	for(i=0;i<5;i++){
		Gen_UKF_U.zk[i]=Data_State.storage_tab_float[j];
		j++;
	}
	/*Put data in Q (the quaterion)*/	
	for(i=0;i<4;i++){
		Gen_UKF_U.q[i]=Data_State.storage_tab_float[j];
		j++;
	}
	/*Put data in Phi*/	
	Gen_UKF_U.phi = Data_State.storage_tab_float[i];
	i++;
	/*Put data in Alpha*/	
	Gen_UKF_U.theta = Data_State.storage_tab_float[i];
	i++;
	printf("Before step\n");
	Gen_UKF_step();
	printf("After step\n");
}
/*------------------get_wind--------------------------*/
/* Fonction will get the out's calcul in the structure*/						
/*----------------------------------------------------*/
void get_wind(){
	int i =0;
	int j;
	
	j=0;
	for(i=i;i<size_of_answer[0];i++){
		Answer_State.storage_tab_float[i] = (float)Gen_UKF_Y.xout[j];
		j++;
	}
	
	j=0;
	for(i=i;i<size_of_answer[1];i++){
		Answer_State.storage_tab_float[i] = (float)Gen_UKF_Y.Pout[j];
		j++;
	}
}
/*----------------------init--------------------------*/
/* Init fonction to init different type of variable   */	
/*  for the calculator before to calculate  		  */					
/*----------------------------------------------------*/
void init(){
	
	fp = fopen (NAME_FILE, "w+");
	
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
	
	
	init_old();
}

/*--------------parse_check_message-------------------*/
/* Fonction call when the message as been recived     */	
/*  and parse message to put data in the structure	  */					
/*----------------------------------------------------*/
static void parse_check_message(IvyClientPtr app, void *user_data, int argc, char *argv[]){
	
	int i=0;
	const char s[2] = ",";
	char *token;
   
	token = strtok(argv[0], s);
    printf("Start \n");
    fprintf(fp, "DATA_RECEIVE:\n");
	while( (token != NULL) && (i<NBR_DATA)) 
	{
		printf( " %s\n", token );
		Data_State.storage_tab_float[i]=(float)atof(token);
		fprintf(fp, "%f\n", Data_State.storage_tab_float[i]);
		token = strtok(NULL, s);
		i++;
	}
	fflush(fp);
	if(i<=(NBR_DATA)){
		parse_data_for_wind_estimation();
		send_wind_estimation();
		printf("\nSend back\n");
	}else{
		send_error();
	}
	free(token);
}
/*----------------------main--------------------------*/
/* 					Main fonction 				      */			
/*----------------------------------------------------*/
int main ( int argc, char** argv) {
  init();
  
  GMainLoop *ml =  g_main_loop_new(NULL, FALSE);
  
  IvyInit ("Rasp", "Wind Estimate READY", 0, 0, 0, 0);
  IvyStart("127.255.255.255");
  IvyBindMsg(parse_check_message, 0,  "^1 PAYLOAD_FLOAT (.*)");
  g_main_loop_run(ml);

  return 0;
}
