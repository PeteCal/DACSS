#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <getopt.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <mosquitto.h>
#include "x345MHz.h"

//extern mosq;
struct mosquitto* mosq = NULL;
	char* MQTT_Host = "192.168.1.115" ;
	int MQTT_Port = 1883;
	char Payload[3];
/* To get this to compile/link I had to add
-lmosquitto
Under Project, Build Options, Linker Settings, Other Link Options.
*/
void createMosquitto(){
	mosquitto_lib_init();
	mosq = mosquitto_new("345Mz",true,NULL);
	mosquitto_loop_start(mosq);
	mosquitto_connect_async(mosq,MQTT_Host,MQTT_Port,60);
	}

void publish(char *Topic, char *action)
{
    mosquitto_publish(mosq,NULL,Topic,strlen(action),action,2,false);
    //printf("Mosquitto Sending: %s %s to %s:%d\n",Topic,action,MQTT_Host,MQTT_Port);
}

