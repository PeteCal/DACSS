#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <getopt.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "x345MHz.h"


int sampleRate=250000;  //Allowable rates 225001 to 300000 and 900001 to 3200000 but 2048000 may be max
int decimateFactor=2;
int decimateCount;//Increment and if = to decimateFactor, pass on the value to mag2word
float filteredI, filteredQ;
float finalMul=100;
int fifoReadSize=500000;
unsigned char mag;
char newEntry[100];
char stringValue[20]; //A numerical value for including in log entry
int logLength=500;
char logFile[500][100];
int nextEntry=0;
char justTime[100];  //This is the string returned by strftime
time_t uct;
struct tm *timeptr;
int sizeOfTime;
bool logRolled=false;
int logOffset=0;
FILE *ramlogA=NULL;
FILE *ramlogB=NULL;
char pathLogA[]="/dev/shm/345MHzRamA.log";
char pathLogB[]="/dev/shm/345MHzRamB.log";
char activeLog[]="ramlogA";
int lastDoorSwitch=0; //=off
int words;

void add2log(char *messageToLog ,int value){

    uct=time(NULL); //Universal Coordinated Time, seconds since 1/1/1970
    timeptr = localtime(&uct);
    sizeOfTime=strftime(justTime,sizeof(justTime),"%D %r  ", timeptr);
    strcpy(newEntry,justTime); //Start with the date and time
    strcat(newEntry,messageToLog); //messageToLog is the second item of the entry
    sprintf(stringValue,"  %i  ",value);  //Converts an integer(=value) to a string array (=stringValue)
    strcat(newEntry,stringValue); //The value is the last item of the entry
    strcat(newEntry,"\n");
    strcpy(logFile[nextEntry],newEntry);
    if (strcmp(activeLog,"ramlogA")==0){
        fprintf(ramlogA,"%s ", logFile[nextEntry]);
        fflush(ramlogA);     // without this nothing is written until fclose
    }else{
        fprintf(ramlogB,"%s ", logFile[nextEntry]);
        fflush(ramlogB);    // without this nothing is written until fclose
    }
    nextEntry++;
    if(nextEntry>=logLength){
    nextEntry=0;
    logRolled=true;
    if(strcmp(activeLog,"ramlogA")==0){
        strcpy(activeLog,"ramlogB");
        fclose(ramlogB);// this should cause the old contents to be cleared when we reopen.
        ramlogB=fopen(pathLogB,"w+");
    }else{
        strcpy(activeLog,"ramlogA");
        fclose(ramlogA);
        ramlogA=fopen(pathLogA,"w+");
        }
    }
    return;
}

int main()
{
unsigned char buffer[fifoReadSize];
int fifoFile;
int IQbytes;
    StartUp();
    printf("\nRunning 345MHz\n");
    ramlogA=fopen(pathLogA,"w+");
    ramlogB=fopen(pathLogB,"w+");
    words=0;
 char* FIFOName = "/dev/shm/stream";
    remove(FIFOName);
  	mkfifo(FIFOName,DEFFILEMODE);
    printf("Sending\n rtl_sdr -f345000000 -s250000 -g49.0  /dev/shm/stream &..\n");
	printf("Waiting for FIFO data...\n");
    system("rtl_sdr -f345000000 -s250000 -g49.0  /dev/shm/stream &");
	fifoFile = open( FIFOName, O_RDONLY );
    printf("Receiving data...\n");
	while( 1 )
	{
		if((IQbytes = read( fifoFile, buffer, fifoReadSize)) > 0)
		{   //printf("Bytes Read= %i  \n", IQbytes); //WE WON'T LOG THIS
            if(IQbytes>300000){ //Typically we get 262144 samples.  Sometimes 258048 followed by 4096 or visa versa
            fprintf(stderr,"^");}
            else{
            fprintf(stderr,".");}
			for(int i=0;i<IQbytes;i=i+2) {
                unsigned char newI=buffer[i];
                unsigned char newQ=buffer[i+1];
                processSample(newI,newQ,&filteredI,&filteredQ);// We call this for every sample so the filter works on every sample
                double magnitude=sqrt(pow(filteredI,2)+pow(filteredQ,2));
                double magXMul=magnitude*finalMul;
                mag=(unsigned char) magXMul;
                decimateCount++;
                if(decimateCount>=decimateFactor){
                    decimateCount=0;
                    mag2word(mag);
                }
            }
		}
		else break;
	}
	add2log("FIFO Closed",0);
//	FILE *log=NULL;
//	log=fopen("345MHzLog.log","w");
//    if(log==NULL){
//        printf("Unable to open log file   \n");
//	}else{
//        if(logRolled==true){
//            for(int logPtr=0;logPtr<logLength;logPtr++){
//                if (nextEntry+logPtr>=logLength){logOffset=logLength;}
//                fprintf(log,"%s",logFile[nextEntry+logPtr-logOffset]);
//         }
//        }else{
//        for(int logPtr=0;logPtr<nextEntry;logPtr++){
//            fprintf(log,"%s",logFile[logPtr]);
//            }
//            }
//        fclose(log);
//        }
fclose(ramlogA);
fclose(ramlogB);
	return 0;
}
