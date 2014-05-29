/*
* Copyright (c) 2005 Jeff Francis <jeff@gritch.org>
* BSD terms apply: see the filr COPYING in the distribution root for details.
*/
 
/*
  Jeff Francis
  jeff@gritch.org
 
  Kind of a curses version of xgps for use with gpsd.
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#ifndef S_SPLINT_S
#include <unistd.h>
#endif /* S_SPLINT_S */
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
 
#include "gps.h"
#include "gpsdclient.h"
#include "revision.h"
 
static struct gps_data_t gpsdata;
static time_t status_timer; /* Time of last state change. */
static int state = 0; /* or MODE_NO_FIX=1, MODE_2D=2, MODE_3D=3 */
static struct fixsource_t source;
#ifdef CLIENTDEBUG_ENABLE
static int debug;
#endif /* CLIENTDEBUG_ENABLE */
 
static bool raw_flag = false;
static bool silent_flag = false;
static bool magnetic_flag = false;
static int window_length;
static int display_sats;
#ifdef TRUENORTH
static bool compass_flag = false;
#endif /* TRUENORTH */
 
/* pseudo-signals indicating reason for termination */
#define CGPS_QUIT 0 /* voluntary yterminastion */
#define GPS_GONE -1 /* GPS device went away */
#define GPS_ERROR -2 /* low-level failure in GPS read */
#define GPS_TIMEOUT -3 /* low-level failure in GPS waiting */


/*Network variables*/
int clientSock;                 /* socket descriptor */
struct sockaddr_in serv_addr;   /* server address structure */
char *servIP;                   /* Server IP address  */
unsigned short servPort;        /* Server Port number */    
char sndBuf[1024];        /* Send Buffer */
//char rcvBuf[RCVBUFSIZE];        /* Receive Buffer */

int formatArray[] = {5,6,8,9,0,1,2,3,11,12,14,15,17,18, 24};
int formatArrayLength = 15;

 
/* Function to call when we're all done. Does a bit of clean-up. */
static void die(int sig)
{
 
  /* We're done talking to gpsd. */
  (void)gps_close(&gpsdata);
 
  switch (sig) {
  case CGPS_QUIT:
  break;
  case GPS_GONE:
  (void)fprintf(stderr, "cgps: GPS hung up.\n");
  break;
  case GPS_ERROR:
  (void)fprintf(stderr, "cgps: GPS read returned error\n");
  break;
  case GPS_TIMEOUT:
  (void)fprintf(stderr, "cgps: GPS timeout\n");
  break;
  default:
  (void)fprintf(stderr, "cgps: caught signal %d\n", sig);
  break;
  }
 
  /* Bye! */
  exit(EXIT_SUCCESS);
 }
 
 static enum deg_str_type deg_type = deg_dd;
 
 int main(int argc, char *argv[])
 {
  int option;
  unsigned int flags = WATCH_ENABLE;
 
  /* Grok the server, port, and device. */
  if (optind < argc) {
  	gpsd_source_spec(argv[optind], &source);
  } else
  	gpsd_source_spec(NULL, &source);
 
  /* Open the stream to gpsd. */
  if (gps_open(source.server, source.port, &gpsdata) != 0) {
  (void)fprintf(stderr,
  "cgps: no gpsd running or network error: %d, %s\n",
  errno, gps_errstr(errno));
  exit(EXIT_FAILURE);
  }
 
  /* note: we're assuming BSD-style reliable signals here */
  (void)signal(SIGINT, die);
  (void)signal(SIGHUP, die);
 
  status_timer = time(NULL);
 
  if (source.device != NULL)
  	flags |= WATCH_DEVICE;
  (void)gps_stream(&gpsdata, flags, source.device);

  /*Setup network socket*/
  servIP = "127.0.0.1";
  servPort = 50007;
  /* Create a new TCP socket*/
  clientSock = socket(AF_INET, SOCK_STREAM, 0);
  printf("Socket ID is: %d \n", clientSock);
  /* Construct the server address structure */
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(servPort);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  /* Establish connecction to the server */
  printf("Attempting to connect\n");
  int connectRet;
  connectRet = connect(clientSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if (connectRet == 0) {
    printf("Connection successful\n");
  } else {
    printf("Connection failed because: %s\n\n", strerror(errno));

  }

    



  /* heart of the client */
  for (;;) {
  	int c;
 
  	if (!gps_waiting(&gpsdata, 5000000)) {
  		die(GPS_TIMEOUT);
  	} else {
  		errno = 0;
  		if (gps_read(&gpsdata) == -1) {
 			fprintf(stderr, "cgps: socket error 4\n");
  			die(errno == 0 ? GPS_GONE : GPS_ERROR);
  		} else {
  		  char rawtime[128];
        char gtime[128];
        char lat[128];
        char lon[123];

        /* Print time/date. */
        if (isnan(gpsdata.fix.time) == 0) {
          (void)unix_to_iso8601(gpsdata.fix.time, rawtime, sizeof(rawtime));
          //printf("%s\n", rawtime);
          int i = 0;
          for (i; i < formatArrayLength; i++) {
            if (i == 8) {
              char num[] = {(rawtime[formatArray[i]]), (rawtime[formatArray[i + 1]])};
              //printf("num %s\n", num);
              int modNum = atoi(num) - 5;
              //printf("%d\n",modNum);
              if (modNum < 0) {
                modNum = 24 - modNum;
                //printf("%d\n", modNum);
              }
              sprintf(num, "%i", modNum);
              gtime[8] = num[0];
              gtime[9] = num[1];
              //gtime[i] = (char) (atoi(gtime[i] + gtime[i]) - 0x4);
            } else if (i == 9){
            } else {
              gtime[i] = rawtime[formatArray[i]];
            }
          }
        } else { 
          (void)snprintf(gtime, sizeof(gtime), "n/a");
        }
        printf("%s", gtime);

        //Fill in latitude
        if (gpsdata.fix.mode >= MODE_2D && isnan(gpsdata.fix.latitude) == 0) {
          (void)snprintf(lat, sizeof(lat), "%s%c",
          deg_to_str(deg_type, fabs(gpsdata.fix.latitude)),
          (gpsdata.fix.latitude < 0) ? 'S' : 'N');
        } else {
          (void)snprintf(lat, sizeof(lat), "n/a");
  		  }
        printf("%s", lat);

 
        /* Fill in the longitude. */
        if (gpsdata.fix.mode >= MODE_2D && isnan(gpsdata.fix.longitude) == 0) {
          (void)snprintf(lon, sizeof(lon), "%s%c",
          deg_to_str(deg_type, fabs(gpsdata.fix.longitude)),
          (gpsdata.fix.longitude < 0) ? 'W' : 'E');
        } else {
          (void)snprintf(lon, sizeof(lon), "n/a");
        }
        printf("%s\n", lon);
        sprintf(sndBuf, "%s%s%s", gtime, lat, lon);
        send(clientSock, &sndBuf, strlen(sndBuf), 0);
        sleep(2);

        /* Send the string to the server */ 
        /* Receive and print response from the server */
        //recv(clientSock, &rcvBuf, sizeof(rcvBuf), 0);
        //sscanf(rcvBuf, "%d", &responseType);
      }
    }
}
}