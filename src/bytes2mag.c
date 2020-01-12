#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <getopt.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include "x345MHz.h"


static int N;
static float coef[4000];  //Assume we will alsways have less then 4000 coefficients
static int last=0; // where the last sample was placed.  For starters, this will change to N-1
static float sampI[4000]={0};//Assume we will alsways have less then 4000 coeficients
static float sampQ[4000]={0}; //Assume we will alsways have less then 4000 coeficients
static float maxNewI=0;
static float minNewI=0;
static float deInterleaveMul=0.01; //multiplier after deinterlieve

int updateLast(int last){
    last=last-1;
    if (last<0){
        last=N-1;
    }
return last;
}
// Read in the coeficients
int getCoeficients(){
    FILE *coefFile;
    char* pathCoef="/home/pi/Documents/DACSS/TapsFIR.flt";
    coefFile=fopen(pathCoef,"rb");
    if (!coefFile){
        printf("Unable to open file TapsFIR.flt  \n");
        return 1;
    }
    printf("\nReading the coefficients file \n");
    float coefs;
    int i=0;
    while(fread(&coefs, sizeof(float),1, coefFile)){
            coef[i]=coefs;
            i=i+1;
    }
    fclose(coefFile);
    N=i; //save the number of coefs.
    printf("Coefficients read %i \n",N);
    return 0;
}

void  processSample(unsigned char newI,unsigned char newQ,float *filteredI,float *filteredQ){
    last=updateLast(last);
    if (newI>maxNewI){maxNewI=newI;}
    if(newI<minNewI){minNewI=newI;}
    //printf("maxNewI= %f    minNewI= %f  \n",maxNewI,minNewI); //For Debug
    float sampItemp=(float)newI;
    sampI[last]=(sampItemp-127)*deInterleaveMul; //reddit said osmocom used 127.4 to get rid of bias.
    float sampQtemp=(float)newQ;
    sampQ[last]=(sampQtemp-127)*deInterleaveMul;
    //we might need some gain here
    *filteredI=0;
    *filteredQ=0;
// Do the filter
    int i=0;
    for(int j=0;j<N;j++){
        *filteredI=*filteredI+sampI[last+i]*coef[j];
        *filteredQ=*filteredQ+sampQ[last+i]*coef[j];
        i++;
        if (last+i>N-1){
            i=-last;
        }
    }
    return;
}
