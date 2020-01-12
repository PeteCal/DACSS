#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <getopt.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "x345MHz.h"

int lastMatch=-1; // sensor code so we won't match before this is changed
int lastSensorCode=0;
extern uint64_t intWord;
char unknown[16];
int lastSensorCount=0;
static char repeatMessage[25];
bool match=false;
int matchCode;
char word[512][18];
char device[512][128];
char action [512][64];
char YN[512][10];
char justFirst[1];
char temp1[10];
int messageNumber=0;
int numWORDS=0;
uint64_t lastWord=0;
char *endptr;
char topic[30]; //for mosquitto
char m[30]; // the message for mosquitto
char temp[100];
int64_t lastUnknownWas=0;
bool lastWasUnknown=false;
char lastUnknownString[21];
void getData(char buff[]);
void getMessageDatabase();

bool    sensorCompare(uint64_t sensorA, uint64_t sensorB){
            if(sensorA==sensorB){
                match=1;
            }else{
                match=0; // to save time, we need next i
            }
        return match;
        }

void word2sensor(uint64_t intWord){   //intWord, uint64_t form of word
    if (intWord==lastWord){ //sensdorCodes[0] contains the last uint64_t
    lastSensorCount++;
    return;}
    else{
    for(int i=0;i<numWORDS;i++){
        matchCode=0;
        int64_t word2Hex=strtoll(word[i],NULL,16);  //Damn strtoll only goes to 0x7FFFFFFFFFFFFFFF
        int64_t shortIntWord=intWord-0XFF00000000000000;  // We already verfied that intWord starts with FF so just compare the rest.
        match=sensorCompare(word2Hex ,shortIntWord);
        if(match){
            matchCode=i;
            break;  //so we don't keep searching
        }
    }
 //Got a new sensor, known or unknown, report how many old ones we got
    if(lastSensorCount!=0){ //If we were counting an old sensor
        if (lastWasUnknown){
        printf("\nRepeat Unknown Code  %llX repeated %i times \n",lastUnknownWas,lastSensorCount);
        strcpy(repeatMessage,"Repeat   ");
        sprintf(lastUnknownString,"%" PRIX64,lastUnknownWas);
        strcat(repeatMessage,lastUnknownString);
       // add2log(repeatMessage,lastSensorCount);
        lastWasUnknown=false;
        }else{
        strcpy(temp,"");
        strcpy(temp,device[lastSensorCode]);
        strcat(temp," ");
        strcat(temp,action[lastSensorCode]);
        printf("\n%s reported %i times \n",temp, lastSensorCount);
        strcpy(repeatMessage,"Repeat  "); // this clears out the previous message
        strcat(repeatMessage,device[lastSensorCode]);
        strcat(repeatMessage," ");
        strcat(repeatMessage,action[lastSensorCode]);
        //add2log(repeatMessage, lastSensorCount);
        }
    }
    lastWord=intWord;
    lastSensorCode=matchCode;
    if(match==true){
    strcpy(temp,"");
    strcat(temp,device[matchCode]);
    strcat(temp," ");
    strcat(temp,action[matchCode]);
    printf("\n%s \n"  ,temp);
    add2log(temp,1);
    strncpy(justFirst,YN[matchCode],1); // length of YN depends on the program used to create WORD2Action.csv
    if (strcmp(justFirst,"Y")==0){      // chop it to 1 character
        strcpy(topic,"");
        strcpy(topic,device[matchCode]);
        strcpy(m,"");
       // strcat(m,"ON");
        strcat(m,action[matchCode]);
        publish(topic, m);
    }
    }else{
    lastUnknownWas=intWord;
    lastWasUnknown=true;
    printf("\nUnknown %llX    \n"  ,intWord);
    strcpy(temp,"Unknown ");
    sprintf(unknown,"%" PRIX64,intWord);
    strcat(temp,unknown);
    add2log(temp,1);
    }

    lastSensorCount=1; //Got a new sensor
    }
            }


void getMessageDatabase()
    {
        const char filename[] = "WORD2Action.csv";
    	FILE *fp = fopen(filename,"r");
        char buff[1024];
        printf("Reading WORD2Action.csv\n");
        while( fgets(buff, 1024, (FILE*)fp)){ //reads to 1024-1 or \n or EOL
            numWORDS++;
            if(numWORDS != 1) //Throw away the header row.
            {
                getData(buff);
                messageNumber++;
            }
    	}
        for(int i=0;i<numWORDS;i++){
        //printf("%s %s %s %s",word[i],device[i],action[i],YN[i]);
       }
        printf("WORD2Action Length= %i \n",numWORDS);
        fclose(fp);
        //let's see if I can find a word
//        char toFind[]="FF7FB5463303C10F";
//        for(int i=0;i<numWORDS;i++){
//            if (!strcmp(word[i],toFind)){
//                printf("The word to find is in %i", i);
//
//                }
//        }
     //    char x =getchar();  //get one character.  Sort of pause before closing the window
    }
    void getData(char buff[])
    {
       char *token = strtok(buff,",");
            strcpy(&word[messageNumber][0],token);
            token = strtok(NULL,","); //NULL means return from buff what is between
                                    //  the previous "," and the next "," then
                                    // point to the next ",".
            strcpy(&device[messageNumber][0],token);
            token = strtok(NULL,",");
            strcpy(&action[messageNumber][0],token);
            token = strtok(NULL,",");
            strcpy(&YN[messageNumber][0],token);
            token = strtok(NULL,",");
    }

/* For debug
for(int i=0;i<10;i++){
    printf("Sensor %s \t %s  \n", sensorName+i,sensorSymbol+i);
}
for (int i=0;i<11;i++){
    for(int j=0;j<8;j++){
        printf("  %i ",sensorCodes[i][j]);
    }
    printf(" \n");
*/
