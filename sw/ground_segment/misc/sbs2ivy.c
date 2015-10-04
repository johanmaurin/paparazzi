/*
 * Copyright (C) 2013 Marc Schwarzbach
 *               2015 Felix Ruess <felix.ruess@gmail.com>
 *
 * This file is part of paparazzi.
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
 *
 */

/**
 * @file sbs2ivy.c
 * Parser for the SBS-1 protocol (ADS-B data).
 *
 *
 * ## Message Types
 *   // Generated when the user changes the selected aircraft in
 *   // BaseStation.
 *   SELECTION_CHANGE: 'SEL',
 *   // Generated when an aircraft being tracked sets or changes its
 *   // callsign.
 *   NEW_ID: 'ID',
 *   // Generated when the SBS picks up a signal for an aircraft that it
 *   // isn't currently tracking,
 *   NEW_AIRCRAFT: 'AIR',
 *   // Generated when an aircraft's status changes according to the
 *   // time-out values in the SBS1 Data Settings menu.
 *   STATUS_CHANGE: 'STA',
 *   // Generated when the user double-clicks (or presses return) on an
 *   // aircraft (i.e. to bring up the aircraft details window).
 *   CLICK: 'CLK',
 *   // Generated by the aircraft. There are eight different MSG
 *   // transmission types, see `TransmissionType`.
 *   TRANSMISSION: 'MSG'
 *
 * ## Transmission Types
 *
 * Transmission messages (MSG) from aircraft may be one of eight types
 * (ES = Extended Squitter, DF = Downlink Format, BDS = B-Definition
 * Subfield).
 *
 * |Type|Description                     |Spec          |
 * |----|--------------------------------|--------------|
 * | 1  | ES identification and category | DF17 BDS 0,8 |
 * | 2  | ES surface position message    | DF17 BDS 0,6 |
 * | 3  | ES airborne position message   | DF17 BDS 0,5 |
 * | 4  | ES airborne velocity message   | DF17 BDS 0,9 |
 * | 5  | Surveillance alt message       | DF4, DF20    |
 * | 6  | Surveillance ID message        | DF5, DF21    |
 * | 7  | Air-to-air message             | DF16         |
 * | 8  | All call reply                 | DF11         |
 *
 */


#include <glib.h>
#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <math.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <signal.h>

#include <Ivy/ivy.h>
#include <Ivy/ivyloop.h>
#include <Ivy/timer.h>
#include <Ivy/version.h>
//#include <Ivy/ivyglibloop.h>

#include "math/pprz_geodetic_int.h"
#include "math/pprz_geodetic_float.h"
#include "math/pprz_geodetic_double.h"
#include "math/pprz_algebra_double.h"

#define DEBUG_INPUT 0
#define DEBUG_INTR 0

#define MAX_INTRUDER 10
#define MAX_AGE_INTR 80   //Scaled by timer, stepsize 1/4s


#if DEBUG_INPUT == 1
#define INPUT_PRINT(...) { printf( __VA_ARGS__);};
#else
#define INPUT_PRINT(...) {  };
#endif


#if DEBUG_INTR == 1
#define INTR_PRINT(...) { printf( __VA_ARGS__);};
#else
#define INTR_PRINT(...) {  };
#endif

#define INPUT_MAXLEN 255

void parse_msg4(void);
void parse_msg3(void);

void handle_intruders(void);

void sbs_parse_msg(void);
void sbs_parse_char(unsigned char c);

void close_port(void);
int open_port(char *host, unsigned int port);
void read_port(void);


struct MsgBuf {
  int msg_available;
  int pos_available;
  int nb_ovrn;        // number if incomplete messages
  char msg_buf[INPUT_MAXLEN];  ///< buffer for storing one line
  int msg_len;
};

struct MsgBuf in_data;

#define NAME_MAXLEN 20
/// data structure for intruders
struct Intruder {
  int id;
  char name[NAME_MAXLEN];
  struct UtmCoor_i utm_pos;
  struct LlaCoor_d lla;
  int lastalt;
  float gspeed;
  float course;
  float climb;
  float dist;
  uint time4;
  uint time3;
  int used;
};

struct Intruder Intr[MAX_INTRUDER + 1];

//Flags
int sendivyflag = 0;

int num_intr;

uint timer = 100;
uint lastivyrcv = 0;
uint lastivytrx = 0;

int portstat = 0;

int dist(struct UtmCoor_i *utmi);


//TCP Port variables
int sockfd;
struct sockaddr_in serv_addr;
char buffer[256];
fd_set readSet;
struct timeval selTimeout;


/** Ivy Bus default */
#ifdef __APPLE__
#define DEFAULT_IVY_BUS "224.255.255.255"
#else
#define DEFAULT_IVY_BUS "127.255.255.255:2010"
#endif

struct Opts {
  uint8_t ac_id;
  char *host;
  unsigned int port;
  unsigned int enable_remote_uav;
  char *ivy_bus;
};

struct Opts opts;


//////////////////////////////////////////////////////////////////////////////////
// SETTINGS
//////////////////////////////////////////////////////////////////////////////////

// Serial Repeat Rate
long delay = 1000;

GtkWidget *status_ivy;
GtkWidget *status_out_ivy;
GtkWidget *status;
GtkWidget *status_sbs;

char status_str[256];
char status_ivy_str[256];
char status_ivy_out[256];
char status_sbs_str[256];

long int count_ivy = 0;
long int count_serial = 0;

//////////////////////////////////////////////////////////////////////////////////
// local_uav DATA
//////////////////////////////////////////////////////////////////////////////////

struct _uav_type_ {
  // Header
  unsigned char header;

  // Data
  unsigned char ac_id;
  int16_t phi, theta, psi;
  uint16_t speed;
  struct LlaCoor_i lla_i;
  int utm_east, utm_north, utm_z;
  unsigned char utm_zone;
  unsigned char pprz_mode;
  float desired_alt;
  int16_t climb;
  int16_t course;
  unsigned char block;

  // Footer
  unsigned char footer;
}
__attribute__((packed))

local_uav;


//////////////////////////////////////////////////////////////////////////////////
// IVY Reader
//////////////////////////////////////////////////////////////////////////////////

static void on_Attitude(IvyClientPtr app, void *user_data, int argc, char *argv[])
{
  /*
     <message name="ATTITUDE" id="6">
       <field name="phi"   type="float" unit="rad" alt_unit="deg"/>
       <field name="psi"   type="float" unit="rad" alt_unit="deg"/>
       <field name="theta" type="float" unit="rad" alt_unit="deg"/>
     </message>
  */

  local_uav.phi   = (short int)(atof(argv[0]) * 1000.0);
  local_uav.psi   = (short int)(atof(argv[1]) * 1000.0);
  local_uav.theta = (short int)(atof(argv[2]) * 1000.0);

}

static void on_Estimator(IvyClientPtr app, void *user_data, int argc, char *argv[])
{
  /*
    <message name="ESTIMATOR" id="42">
      <field name="z" type="float" unit="m"/>
      <field name="z_dot" type="float" unit="m/s"/>
    </message>
  */

  local_uav.utm_z = ((atof(argv[0])) * 1000.0f);
  local_uav.climb = atof(argv[1]);

}

static void on_Navigation(IvyClientPtr app, void *user_data, int argc, char *argv[])
{
  /*
     <message name="NAVIGATION" id="10">
       <field name="cur_block" type="uint8"/>
       <field name="cur_stage" type="uint8"/>
       <field name="pos_x" type="float" unit="m" format="%.1f"/>
       <field name="pos_y" type="float" unit="m" format="%.1f"/>
       <field name="dist2_wp" type="float" format="%.1f" unit="m^2"/>
       <field name="dist2_home" type="float" format="%.1f" unit="m^2"/>
       <field name="circle_count" type="uint8"/>
       <field name="oval_count" type="uint8"/>
     </message>
  */

  local_uav.block = atoi(argv[0]);
}

static void on_Desired(IvyClientPtr app, void *user_data, int argc, char *argv[])
{
  /*
     <message name="DESIRED" id="16">
       <field name="roll" type="float" format="%.2f" unit="rad" alt_unit="deg" alt_unit_coef="57.3"/>
       <field name="pitch" type="float" format="%.2f" unit="rad" alt_unit="deg" alt_unit_coef="57.3"/>
       <field name="course" type="float" format="%.1f" unit="rad" alt_unit="deg" alt_unit_coef="57.3"/>
       <field name="x" type="float" format="%.0f"  unit="m"/>
       <field name="y" type="float" format="%.0f" unit="m"/>
       <field name="altitude" type="float" format="%.0f"  unit="m"/>
       <field name="climb" type="float" format="%.1f"  unit="m/s"></field>
       <field name="airspeed" type="float" format="%.1f"  unit="m/s"></field>
     </message>

  */
  local_uav.desired_alt = atof(argv[5]);
}

static void on_Gps(IvyClientPtr app, void *user_data, int argc, char *argv[])
{
  /*
     <message name="GPS" id="8">
       <field name="mode"       type="uint8"  unit="byte_mask"/>
       <field name="utm_east"   type="int32"  unit="cm" alt_unit="m"/>
       <field name="utm_north"  type="int32"  unit="cm" alt_unit="m"/>
       <field name="course"     type="int16"  unit="decideg" alt_unit="deg"/>
       <field name="alt"        type="int32"  unit="mm" alt_unit="m"/>
       <field name="speed"      type="uint16" unit="cm/s" alt_unit="m/s"/>
       <field name="climb"      type="int16"  unit="cm/s" alt_unit="m/s"/>
       <field name="week"       type="uint16" unit="weeks"/>
       <field name="itow"       type="uint32" unit="ms"/>
       <field name="utm_zone"   type="uint8"/>
       <field name="gps_nb_err" type="uint8"/>
     </message>

  */
  local_uav.utm_east = atoi(argv[1]);
  local_uav.utm_north = atoi(argv[2]);
  local_uav.utm_zone = atoi(argv[9]);
  local_uav.speed = atoi(argv[5]);

  count_ivy++;
  lastivyrcv = timer;

  //Print to window
  sprintf(status_ivy_str, "Received from IVY: [%ld]", count_ivy);
  gtk_label_set_text(GTK_LABEL(status_ivy), status_ivy_str);
}

//////////////////////////////////////////////////////////////////////////////////
// IVY Writer
//////////////////////////////////////////////////////////////////////////////////

void send_intruder(struct Intruder *intruder)
{
  /*
  <message name="INTRUDER" id="37">
    <field name="id"     type="string"/>
    <field name="name"   type="string"/>
    <field name="lat"    type="int32"  unit="1e7deg" alt_unit="deg" alt_unit_coef="0.0000001"/>
    <field name="lon"    type="int32"  unit="1e7deg" alt_unit="deg" alt_unit_coef="0.0000001"/>
    <field name="alt"    type="int32"  unit="mm" alt_unit="m">altitude above WGS84 reference ellipsoid</field>
    <field name="course" type="float"  unit="deg"/>
    <field name="speed"  type="float" unit="m/s"/>
    <field name="climb"  type="float"  unit="m/s"/>
    <field name="itow"   type="uint32" unit="ms"/>
  </message>
  */

  struct LlaCoor_i lla_i;
  LLA_BFP_OF_REAL(lla_i, intruder->lla);

  // FIXME: using WGS84 ellipsoid alt, it is probably hmsl???
  IvySendMsg("INTRUDER %d %s %d %d %d %f %f %f %d\n", intruder->id, intruder->name,
             lla_i.lat, lla_i.lon, lla_i.alt, intruder->course,
             intruder->gspeed, intruder->climb, 0);

  count_serial++;
  lastivytrx = timer;

  sprintf(status_ivy_out, "Sending Intruder ID: %d  [%ld]", intruder->id, count_serial);
  gtk_label_set_text(GTK_LABEL(status_out_ivy), status_ivy_out);
}


void send_remote_uav(struct Intruder *intruder)
{

  struct _uav_type_ remote_uav;

  remote_uav.ac_id = 254;
  remote_uav.phi = 0;
  LLA_BFP_OF_REAL(remote_uav.lla_i, Intr->lla);
  remote_uav.utm_east = Intr->utm_pos.east;
  remote_uav.utm_north = Intr->utm_pos.north;
  remote_uav.utm_z = Intr->utm_pos.alt;
  remote_uav.utm_zone = Intr->utm_pos.zone;
  // uav speed/climb are in in cm/s
  remote_uav.speed = Intr->gspeed * 100;
  remote_uav.climb = Intr->climb * 100;
  // course in decideg
  remote_uav.course = Intr->course * 10;


  IvySendMsg("%d ALIVE 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n", remote_uav.ac_id);

  // ATTITUDE: phi, psi, theta
  float psi = RadOfDeg(Intr->course);
  IvySendMsg("%d ATTITUDE %f %f %f\n", remote_uav.ac_id, 0.0f, psi, 0.0f);


  /*
     <message name="GPS" id="8">
       <field name="mode"       type="uint8"  unit="byte_mask"/>
       <field name="utm_east"   type="int32"  unit="cm" alt_unit="m"/>
       <field name="utm_north"  type="int32"  unit="cm" alt_unit="m"/>
       <field name="course"     type="int16"  unit="decideg" alt_unit="deg"/>
       <field name="alt"        type="int32"  unit="mm" alt_unit="m"/>
       <field name="speed"      type="uint16" unit="cm/s" alt_unit="m/s"/>
       <field name="climb"      type="int16"  unit="cm/s" alt_unit="m/s"/>
       <field name="week"       type="uint16" unit="weeks"/>
       <field name="itow"       type="uint32" unit="ms"/>
       <field name="utm_zone"   type="uint8"/>
       <field name="gps_nb_err" type="uint8"/>
     </message>

  IvySendMsg("%d GPS 3 %d %d %d %d %d %d 0 0 %d 0\n", remote_uav.ac_id, remote_uav.utm_east, remote_uav.utm_north,
             (int)remote_uav.course, remote_uav.utm_z, remote_uav.speed, (int)remote_uav.climb, remote_uav.utm_zone);

  */

  /*
    <message name="GPS_LLA" id="59">
    <field name="lat"        type="int32"  unit="1e7deg" alt_unit="deg" alt_unit_coef="0.0000001"/>
    <field name="lon"        type="int32"  unit="1e7deg" alt_unit="deg" alt_unit_coef="0.0000001"/>
    <field name="alt"        type="int32"  unit="mm" alt_unit="m">altitude above WGS84 reference ellipsoid</field>
    <field name="hmsl"       type="int32"  unit="mm" alt_unit="m">Height above Mean Sea Level (geoid)</field>
    <field name="course"     type="int16"  unit="decideg" alt_unit="deg"/>
    <field name="speed"      type="uint16" unit="cm/s" alt_unit="m/s"/>
    <field name="climb"      type="int16"  unit="cm/s" alt_unit="m/s"/>
    <field name="week"       type="uint16" unit="weeks"/>
    <field name="itow"       type="uint32" unit="ms"/>
    <field name="mode"       type="uint8"  unit="byte_mask"/>
    <field name="gps_nb_err" type="uint8"/>
  </message>
  */

  // FIXME: using alt from ADS-B as ellipsoid and geoid alt...
  IvySendMsg("%d GPS_LLA %d %d %d %d %d %d %d 0 0 3 0\n", remote_uav.ac_id,
             remote_uav.lla_i.lat, remote_uav.lla_i.lon, remote_uav.lla_i.alt,
             remote_uav.lla_i.alt, remote_uav.course, remote_uav.speed, remote_uav.climb);

  /*
    <message name="FBW_STATUS" id="103">
      <field name="rc_status" type="uint8" values="OK|LOST|REALLY_LOST"/>
      <field name="frame_rate" type="uint8" unit="Hz"/>
      <field name="mode" type="uint8" values="MANUAL|AUTO|FAILSAFE"/>
      <field name="vsupply" type="uint8" unit="decivolt"/>
      <field name="current" type="int32" unit="mA"/>
    </message>
  */

  IvySendMsg("%d FBW_STATUS 2 0 1 81 0 \n", remote_uav.ac_id);

  float z = ((float)remote_uav.utm_z) / 1000.0f;
  float zdot = remote_uav.climb / 100.0f;
  IvySendMsg("%d ESTIMATOR %f %f\n", remote_uav.ac_id, z, zdot);

  count_serial++;
  lastivytrx = timer;

  sprintf(status_ivy_out, "Intruder ID: %d; forwarding to IVY [%ld]", remote_uav.ac_id, count_serial);
  gtk_label_set_text(GTK_LABEL(status_out_ivy), status_ivy_out);
}



//////////////////////////////////////////////////////////////////////////////////
// TIMER
//////////////////////////////////////////////////////////////////////////////////

// Main functions called every 1/4s
gboolean timeout_callback(gpointer data)
{
  static unsigned char dispatch = 0;

  // Every Time
  if (portstat == 1) {
    read_port();
  }
  sendivyflag = 1;
  handle_intruders();
  timer++;

  // One out of 4
  if (dispatch > 2) {
    dispatch = 0;

    if ((timer - lastivyrcv) > 10) {
      sprintf(status_ivy_str, "--");
      gtk_label_set_text(GTK_LABEL(status_ivy), status_ivy_str);
    }
    if ((timer - lastivytrx) > 10) {
      sprintf(status_ivy_out, "--");
      gtk_label_set_text(GTK_LABEL(status_out_ivy), status_ivy_out);
    }
    if (portstat == 0) {
      portstat = open_port(opts.host, opts.port);
    }

  } else {
    dispatch ++;
  }
  return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// MAIN
//////////////////////////////////////////////////////////////////////////////////

gint delete_event(GtkWidget *widget,
                  GdkEvent  *event,
                  gpointer   data)
{
  g_print("CLEAN STOP\n");

  close_port();
  IvyStop();

  exit(0);

  return (FALSE); // false = delete window, FALSE = keep active
}


static bool_t parse_options(int argc, char **argv, struct Opts *opts)
{
  opts->ac_id = 0;
  opts->host = "localhost";
  opts->port = 30003;
  opts->enable_remote_uav = 0;
  opts->ivy_bus = DEFAULT_IVY_BUS;

  static const char *usage =
    "Usage: %s [options]\n"
    " Options :\n"
    "   -h                           Display this help\n"
    "   --ac <ac_id>                 e.g. 23\n"
    "   --host <hostname>            e.g. localhost\n"
    "   --port <port>                e.g. 30002\n"
    "   --enable_remote_uav\n"
    "   --ivy_bus <ivy bus>          e.g. 127.255.255.255\n";

  while (1) {

    static struct option long_options[] = {
      {"ac", 1, NULL, 0},
      {"host", 1, NULL, 0},
      {"port", 1, NULL, 0},
      {"enable_remote_uav", 0, NULL, 0},
      {"ivy_bus", 1, NULL, 0},
      {0, 0, 0, 0}
    };
    int option_index = 0;
    int c = getopt_long(argc, argv, "h", long_options, &option_index);
    if (c == -1) {
      break;
    }

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            opts->ac_id = atoi(optarg); break;
          case 1:
            opts->host = strdup(optarg); break;
          case 2:
            opts->port = atoi(optarg); break;
          case 3:
            opts->enable_remote_uav = 1; break;
          case 4:
            opts->ivy_bus = strdup(optarg); break;
          default:
            break;
        }
        break;

      case 'h':
        fprintf(stderr, usage, argv[0]);
        exit(0);

      default: /* ’?’ */
        printf("?? getopt returned character code 0%o ??\n", c);
        fprintf(stderr, usage, argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  return TRUE;
}

int main(int argc, char **argv)
{

  if (!parse_options(argc, argv, &opts)) { return 1; }

  gtk_init(&argc, &argv);

  local_uav.ac_id = opts.ac_id;

  sprintf(status_str, "Listening to AC=%d", local_uav.ac_id);
  sprintf(status_ivy_str, "--");
  sprintf(status_ivy_out, "--");
  printf("%s\n", status_str);

  printf("Listening for SBS-1 messages on %s:%d\n", opts.host, opts.port);
  portstat = open_port(opts.host, opts.port);

  // Start IVY
  IvyInit("SBS2Ivy", "SBS2Ivy READY", NULL, NULL, NULL, NULL);
  IvyBindMsg(on_Desired, NULL, "^%d DESIRED (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)", local_uav.ac_id);
  IvyBindMsg(on_Estimator, NULL, "^%d ESTIMATOR (\\S*) (\\S*)", local_uav.ac_id);
  IvyBindMsg(on_Navigation, NULL, "^%d NAVIGATION (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)",
             local_uav.ac_id);
  IvyBindMsg(on_Attitude, NULL, "^%d ATTITUDE (\\S*) (\\S*) (\\S*)", local_uav.ac_id);
  IvyBindMsg(on_Gps, NULL, "^%d GPS (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)",
             local_uav.ac_id);
  IvyStart("127.255.255.255");

  // Add Timer
  gtk_timeout_add(delay / 4, timeout_callback, NULL);

  // GTK Window
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "SBS2Ivy");

  gtk_signal_connect(GTK_OBJECT(window), "delete_event",
                     GTK_SIGNAL_FUNC(delete_event), NULL);

  GtkWidget *box = gtk_vbox_new(TRUE, 1);
  gtk_container_add(GTK_CONTAINER(window), box);

  GtkWidget *hbox = gtk_hbox_new(FALSE, 1);
  gtk_container_add(GTK_CONTAINER(box), hbox);
  status = gtk_label_new("Status:");
  gtk_box_pack_start(GTK_BOX(hbox), status, FALSE, FALSE, 1);
  gtk_label_set_justify((GtkLabel *) status, GTK_JUSTIFY_LEFT);
  status = gtk_label_new(status_str);
  gtk_box_pack_start(GTK_BOX(hbox), status, FALSE, FALSE, 1);
  gtk_label_set_justify((GtkLabel *) status, GTK_JUSTIFY_LEFT);

  hbox = gtk_hbox_new(FALSE, 1);
  gtk_container_add(GTK_CONTAINER(box), hbox);
  status_ivy = gtk_label_new("From IVY:");
  gtk_box_pack_start(GTK_BOX(hbox), status_ivy, FALSE, FALSE, 1);
  gtk_label_set_justify((GtkLabel *) status_ivy, GTK_JUSTIFY_LEFT);
  status_ivy = gtk_label_new(status_ivy_str);
  gtk_box_pack_start(GTK_BOX(hbox), status_ivy, FALSE, FALSE, 1);
  gtk_label_set_justify((GtkLabel *) status_ivy, GTK_JUSTIFY_LEFT);

  hbox = gtk_hbox_new(FALSE, 1);
  gtk_container_add(GTK_CONTAINER(box), hbox);
  status_out_ivy = gtk_label_new("To IVY:");
  gtk_box_pack_start(GTK_BOX(hbox), status_out_ivy, FALSE, FALSE, 1);
  gtk_label_set_justify((GtkLabel *) status_out_ivy, GTK_JUSTIFY_LEFT);
  status_out_ivy = gtk_label_new(status_ivy_out);
  gtk_label_set_justify(GTK_LABEL(status_out_ivy), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start(GTK_BOX(hbox), status_out_ivy, FALSE, FALSE, 1);

  hbox = gtk_hbox_new(FALSE, 1);
  gtk_container_add(GTK_CONTAINER(box), hbox);
  status_sbs = gtk_label_new("SBS:");
  gtk_box_pack_start(GTK_BOX(hbox), status_sbs, FALSE, FALSE, 1);
  gtk_label_set_justify((GtkLabel *) status_sbs, GTK_JUSTIFY_LEFT);
  status_sbs = gtk_label_new(status_sbs_str);
  gtk_label_set_justify(GTK_LABEL(status_sbs), GTK_JUSTIFY_LEFT);
  gtk_box_pack_start(GTK_BOX(hbox), status_sbs, FALSE, FALSE, 1);


  gtk_widget_show_all(window);

  gtk_main();

  // Clean up
  fprintf(stderr, "Stopping\n");

  return 0;
}

///
//Subroutines
/////

//////////////////////////////////////////////////////////////////////////////////
// TCP PORT
//////////////////////////////////////////////////////////////////////////////////

/// Open
int open_port(char *host, unsigned int port)
{

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if ((sockfd < 0) && DEBUG_INPUT) {
    perror("ERROR opening socket");
  }

  struct hostent *server;
  server = gethostbyname(host);

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(port);
  if ((connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) && DEBUG_INPUT) {
    perror("ERROR connecting socket");
    return 0;
  }

  //set values for select()
  FD_ZERO(&readSet);
  FD_SET(sockfd, &readSet);//tcp socket

  selTimeout.tv_sec = 0;       /* timeout (secs.) */
  selTimeout.tv_usec = 10;     /* 10 microseconds */

  printf("Connected to %s, port %i\n", host, port);

  return 1;
}

///Close
void close_port(void)
{
  close(sockfd);
}

/////////////////////////////////////
//Read ADSB data from TCP port
////////////////////////////////////////////

void read_port(void)
{
  unsigned char readbuf;
  int sel_ret;

  FD_SET(sockfd, &readSet);//set tcp socket for select

  sel_ret = select(sockfd + 1, &readSet, NULL, NULL, &selTimeout);

  if (sel_ret == -1) {
    perror("select"); // error occurred in select()
  } else if (sel_ret == 0) {
    INPUT_PRINT(".");  //Debug for no message
    fflush(stdout);
  } else {
    FD_SET(sockfd, &readSet);//re-set
    while (select(sockfd + 1, &readSet, NULL, NULL, &selTimeout) > 0) {
      FD_SET(sockfd, &readSet);//re-set
      if (read(sockfd, &readbuf, 1) == 1) {
        sbs_parse_char(readbuf);
      } else { //port closed at server, go to reconnect
        portstat = 0;
        close_port();
        break;
      }
      if (in_data.msg_available) {
        sbs_parse_msg();
      }
    }
  }

}

/**
 * This is the actual parser.
 * It reads one character at a time
 * setting in_data.msg_available to TRUE
 * after a full line.
 */
void sbs_parse_char(unsigned char c)
{
  //reject empty lines
  if (in_data.msg_len == 0) {
    if (c == '\r' || c == '\n' || c == '$') {
      return;
    }
  }

  // fill the buffer, unless it's full
  if (in_data.msg_len < INPUT_MAXLEN - 1) {

    // messages end with a linefeed
    //AD: TRUNK:       if (c == '\r' || c == '\n')
    if (c == '\r' || c == '\n') {
      in_data.msg_available = 1;
    } else {
      in_data.msg_buf[in_data.msg_len] = c;
      in_data.msg_len ++;
      in_data.msg_available = 0;
    }
  }

  if (in_data.msg_len >= INPUT_MAXLEN - 1) {
    in_data.msg_available = 1;
  }
}


/**
 * sbs_parse_char() has a complete line.
 * Find out what type of message it is and
 * hand it to the parser for that type.
MSG,4,,,495224,,,,,,,,404,54,,,64,,0,0,0,0
MSG,1,,,495224,,,,,,TAP594  ,,,,,,,,0,0,0,0
MSG,8,,,495224,,,,,,,,,,,,,,,,,
MSG,3,,,495224,,,,,,,37000,,,48.27750,11.68840,,,0,0,0,0
 */
void sbs_parse_msg(void)
{

  if (in_data.msg_len > 5 && !strncmp(in_data.msg_buf , "MSG,4", 5)) {
    in_data.msg_buf[in_data.msg_len] = 0;
    INPUT_PRINT("parsing MSG 4: \"%s\" \n\r", in_data.msg_buf);
    parse_msg4();
  } else {
    if (in_data.msg_len > 5 && !strncmp(in_data.msg_buf , "MSG,3", 5)) {
      in_data.msg_buf[in_data.msg_len] = 0;
      INPUT_PRINT("parsing MSG 3: \"%s\" \n\r", in_data.msg_buf);
      parse_msg3();
    } else {
      in_data.msg_buf[in_data.msg_len] = 0;
      INPUT_PRINT("ignoring: len=%i \"%s\" \n\r", in_data.msg_len, in_data.msg_buf);
    }
  }

  // reset message-buffer
  in_data.msg_len = 0;
}


static inline void read_until_sep(int *i)
{
  while (in_data.msg_buf[(*i)++] != ',') {
    if (*i >= in_data.msg_len) {
      return;
    }
  }
}


/**
 * parse MSG 4 SBS airborne velocity message
 *
 * MSG,4,0,1019,3C65A3,1119,2015/08/14,18:19:17.913,2015/08/14,18:19:17.913,,,511.6,48.6,,,128,,,,,
 */
void parse_msg4(void)
{
  int i = 6;     // current position in the message, start after: MSG,4,
  char *endptr;  // end of parsed substrings

  // transmission type
  //int type = atoi(&in_data.msg_buf[4]);
  //if (type != 4) { return; }

  // aircraft ID
  read_until_sep(&i);

  // hex ident
  read_until_sep(&i);
  // Reading Hex number
  in_data.msg_buf[i - 2] = '0';
  in_data.msg_buf[i - 1] = 'x';
  //get Flarm intruder ID
  int id = strtod(&in_data.msg_buf[i - 2], &endptr);
  INPUT_PRINT("MSG4 id = %i \n\r", id);

  // skip to ground speed
  // flight_id, generated_date, generated_time, logged_date, logged_time, ?, ?
  int j;
  for (j = 0; j < 8; j++) {
    read_until_sep(&i);
  }

  //get intruder Ground speed (knots) and convert to m/s
  double speed = strtod(&in_data.msg_buf[i], &endptr) * 0.5144444;
  INPUT_PRINT("Speed = %f m/s\n\r", speed);

  //get intruder ground track
  read_until_sep(&i);
  double track = strtod(&in_data.msg_buf[i], &endptr);
  INPUT_PRINT("Track = %f deg\n\r", track);

  // next field: empty
  read_until_sep(&i);

  // next field: empty
  read_until_sep(&i);

  // next field: Climb rate
  read_until_sep(&i);

  //get intruder climb rate (feet per minute) and convert to m/s
  double climb = strtod(&in_data.msg_buf[i], &endptr) * 0.00508;
  INPUT_PRINT("Climb = %f m/s\n\r", climb);

  //Build table of current intruder situation

  //Check if already in list, then replace by new values
  int z ;
  int newflag = 1;

  for (z = 0; z < MAX_INTRUDER; z++) {
    if (Intr[z].id == id) {
      newflag = 0;
      //Check for positive or negative vertical speed (not signed :-( )
      if (Intr[z].utm_pos.alt < Intr[z].lastalt) {
        //sinking
        climb = -climb;
      } else if (Intr[z].utm_pos.alt == Intr[z].lastalt) {
        //equal.. take last
        if (Intr[z].climb < 0) {
          climb = -climb;
        }
      }

      Intr[z].gspeed = speed;
      Intr[z].course = track;
      Intr[z].climb  = climb;
      Intr[z].time4  = timer;
    }

  }

  //New intruder
  if (newflag) {
    Intr[MAX_INTRUDER].id     = id;
    Intr[MAX_INTRUDER].gspeed = speed;
    Intr[MAX_INTRUDER].course = track;
    Intr[MAX_INTRUDER].climb  = climb;
    Intr[MAX_INTRUDER].time4  = timer;
    Intr[MAX_INTRUDER].used   = 4;
    INPUT_PRINT("new = %i \n\r", Intr[MAX_INTRUDER].id);

  }

  handle_intruders();

  return;
}


/**
 * parse MSG 3 SBS Message (airborne position message)
 *
 * MSG,3,0,1019,3C65A3,1119,2015/08/14,18:18:21.714,2015/08/14,18:18:21.714,,35000,,,52.81230,13.39689,,,,0,0,0
 *
 * message_type
 * transmission_type
 * session_id
 * aircraft_id = parts[3];
 * hex_ident = parts[4];
 * flight_id = parts[5];
 * generated_date = parts[6];
 * generated_time = parts[7];
 * logged_date = parts[8];
 * logged_time = parts[9];
 * callsign = parts[10];
 * altitude = sbs1_value_to_int(parts[11]);
 * ground_speed = sbs1_value_to_int(parts[12]);
 * track = sbs1_value_to_int(parts[13]);
 * lat = sbs1_value_to_float(parts[14]);
 * lon = sbs1_value_to_float(parts[15]);
 * vertical_rate = sbs1_value_to_int(parts[16]);
 * squawk = parts[17];
 * alert = sbs1_value_to_bool(parts[18]);
 * emergency = sbs1_value_to_bool(parts[19]);
 * spi = sbs1_value_to_bool(parts[20]);
 * is_on_ground = sbs1_value_to_bool(parts[21]);
 */
void parse_msg3(void)
{

  int i = 6;     // current position in the message, start after: MSG,4,
  char *endptr;  // end of parsed substrings
  struct LlaCoor_d lla;
  struct UtmCoor_f utmf;
  struct UtmCoor_i utmi;

  // transmission type
  //int type = atoi(&in_data.msg_buf[4]);
  //if (type != 3) { return; }

  // aircraft ID
  read_until_sep(&i);

  // hex ident
  read_until_sep(&i);
  //Reading Hex number
  in_data.msg_buf[i - 2] = '0';
  in_data.msg_buf[i - 1] = 'x';
  //get Flarm intruder ID
  int id = strtod(&in_data.msg_buf[i - 2], &endptr);
  INPUT_PRINT("MSG3 id = %i \n\r", id);

  // skip some fields
  // flight_id, generated_date, generated_time, logged_date, logged_time, callsign
  int j;
  for (j = 0; j < 6; j++) {
    read_until_sep(&i);
  }

  //get intruder alt (in feet)
  read_until_sep(&i);
  lla.alt = strtod(&in_data.msg_buf[i], &endptr) * 0.3048; //feet to m
  INPUT_PRINT("Alt = %f m\n\r", lla.alt);

  // ground_speed
  read_until_sep(&i);

  // track
  read_until_sep(&i);

  // latitude
  read_until_sep(&i);
  double lat = strtod(&in_data.msg_buf[i], &endptr);
  INPUT_PRINT("Lat = %f deg\n\r", lat);
  lla.lat = RadOfDeg(lat);

  // longitude
  read_until_sep(&i);
  double lon = strtod(&in_data.msg_buf[i], &endptr);
  INPUT_PRINT("Lon = %f deg\n\r", lon);
  lla.lon = RadOfDeg(lon);

  // vertical rate
  read_until_sep(&i);

  // not using the rest of the fields atm


  // FIXME, LLA should not be stored as float!
  struct LlaCoor_f lla_f;
  LLA_COPY(lla_f, lla);
  struct LlaCoor_i lla_i;
  LLA_BFP_OF_REAL(lla_i, lla);

  //Calculations for UTM zone of local UAV so that distance calc works even for zone borders
  utmf.zone = local_uav.utm_zone;
  utm_of_lla_f(&utmf, &lla_f);

  /* copy results of utm conversion */
  utmi.east = utmf.east * 100;
  utmi.north = utmf.north * 100;
  utmi.alt = lla_i.alt;
  utmi.zone = utmf.zone;

  INPUT_PRINT("UTMI E N Z = %d, %d, %d, %d \n\r", utmi.east, utmi.north, utmi.zone,  dist(&utmi));


  //Build table of current intruder situation

  //Check if already in list, then replace by new values
  int z ;
  int newflag = 1;

  for (z = 0; z < MAX_INTRUDER; z++) {
    if (Intr[z].id == id) {
      newflag = 0;
      Intr[z].lastalt  = Intr[z].utm_pos.alt;
      Intr[z].lla      = lla;
      Intr[z].utm_pos  = utmi;
      Intr[z].dist     = dist(&utmi);
      Intr[z].time3    = timer;
    }

  }

  //New intruder
  //If Relative East is empty (value 0), it is a mode C/S transponder, so no position.
  if (newflag) {
    Intr[MAX_INTRUDER].id       = id;
    Intr[MAX_INTRUDER].lla      = lla;
    Intr[MAX_INTRUDER].utm_pos  = utmi;
    Intr[MAX_INTRUDER].dist     = dist(&utmi);
    Intr[MAX_INTRUDER].time3    = timer;
    Intr[MAX_INTRUDER].used     = 3;
    INPUT_PRINT("new = %i \n\r", Intr[MAX_INTRUDER].id);

  }

  handle_intruders();
  return;
}

//Build up of intruder table
void handle_intruders(void)
{

  int z;

  //Analyse if Data ok
  num_intr = 0;
  for (z = 0; z < MAX_INTRUDER; z++) {
    //If data too old, mark unused
    if (((timer - Intr[z].time3) < MAX_AGE_INTR) && ((timer - Intr[z].time4) < MAX_AGE_INTR)) {
      Intr[z].used = 1 ;
      num_intr++; //count valid planes
    } else if ((timer - Intr[z].time3) < MAX_AGE_INTR) {
      Intr[z].used = 3 ;
    } else if ((timer - Intr[z].time4) < MAX_AGE_INTR) {
      Intr[z].used = 4 ;
    } else {
      Intr[z].used = 0;
    }
  }

  INTR_PRINT("unsort %i", num_intr);
  for (z = 0; z < MAX_INTRUDER + 1; z++) {
    INTR_PRINT("Nr:%i d:%.2f u:%d", z, Intr[z].dist, Intr[z].used);
  }
  INTR_PRINT("fin \n\r");

  //Sort for dist and used
  int zi;
  struct Intruder Temp_int;
  //Bubble sort
  for (z = 0; z < MAX_INTRUDER + 1; z++) {
    for (zi = 0; zi < MAX_INTRUDER + 1; zi++) {
      if (((Intr[zi].dist > Intr[zi + 1].dist) && (Intr[zi + 1].used == 1)) || (Intr[zi].used == 0)) {
        Temp_int = Intr[zi];
        Intr[zi] = Intr[zi + 1];
        Intr[zi + 1] = Temp_int;
      }
    }
  }

  if (sendivyflag) {
    for (z = 0; z < MAX_INTRUDER + 1; z++) {
      if (Intr[z].used == 1) {
        if (opts.enable_remote_uav && z == 0) {
          send_remote_uav(&Intr[0]);
        }

        send_intruder(&Intr[z]);
      }
    }
    sendivyflag = 0;
  }

  //show on window
  float dist_close = 0.0;
  if (Intr[0].used == 1) {
    dist_close = Intr[0].dist / 100000.0;
  }

  if (portstat == 1) {
    sprintf(status_sbs_str, "Connected; Intruders: %i  Min.Dist: %4.1f km", num_intr, dist_close);
    gtk_label_set_text(GTK_LABEL(status_sbs), status_sbs_str);
  } else {
    sprintf(status_sbs_str, "No SBS Data, Port closed");
    gtk_label_set_text(GTK_LABEL(status_sbs), status_sbs_str);
  }
  return;
}

//calculate distance intruder to local UAV
int dist(struct UtmCoor_i *utmi)
{
  int d = sqrtf((float)(utmi->north - local_uav.utm_north) * (float)(utmi->north - local_uav.utm_north) +
                (float)(utmi->east - local_uav.utm_east) * (float)(utmi->east - local_uav.utm_east) +
                ((float)(utmi->alt - local_uav.utm_z) / 10.0) * ((float)(utmi->alt - local_uav.utm_z) / 10.0));

  return d;
}
