#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <getopt.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include "x345MHz.h"
/*
I discovered there are noise bursts with many traansisitions.  Some 1ms long.  I will revise the manchester decoding:
pulseWidth=270us.  Subject to change.
There is a counter samplesSinceMid:
There is a samplesPerBit=pulseWidth*sampleRate/decimateFactor
tol = 0.1*samplesPerBit  Tollerance to
Wait for first rise and bitStart=samplesSinceMid-samples/Bit/2
afterMid=mag (which is true the first time)
bitEnd=bitStart+samplesPerBit
Every sample must remain same until bitEnd-tol
If so, store a 1.  If not, see flush.
beforeMid=value at bitEnd+tol
nextGate=bitStart+samplesPerBit/2-tol
every sample must equal beforeMid until nextGate.
Between nextGate and nextGate+2*tol there must be a change.
    if not, flush
    if yes, H>L=0. L>H=1 bitStart=samplesSinceMid-samplesPerBit/2  Re establish sync
    afterMid=



*/


    static float pulseWidth=270e-6; //263.5e-6 ;//264e-6; //270e-6   //In microseconds of the origonal signal before sampleing and decemating
    extern int decimateFactor;
    extern int sampleRate;
    extern int words;
    int samplesPerBit;
    //With no decimation the pulse train is 17.28 ms long or 8 bytes x 8 bits x 270 samples=17.28k samples
    //Narrow pulse is ~135 samples
    static int threshold =32;//Above this is a "1"
    static int samplesSinceMid=0; //from the data file
//    static int samplesSinceBit=0;  //Samples since last bit was saved
    bool level=false;
    bool afterMid;
    bool beforeMid;
    int bitStart;
    int bitEnd;
    int nextGate;
    int bytes;
    bool gotChange=false;
    int tol;
    static bool sampling = false;
    static int bits = 0; //  a count of bits received
    static bool newByte[8]= {false,false,false,false,false,false,false,false}; // the byte we are building
    uint64_t  intWord=0;
    unsigned char magarray[1000];
    int magCount=0;
void flush(int type);
    unsigned char bool2bin(bool newByte[]){
    int bit=0;
    int byteValue=0;
    while (bit<8){
        if (newByte[bit]==true){
            byteValue=byteValue+pow(2,bit);
        }
        bit+=1;
    }
    return byteValue;
}
void mag2word(unsigned int mag){
//    printf("Started mag2word %i   \n", mag);
    samplesPerBit=pulseWidth*sampleRate/decimateFactor+1;
    tol=0.1*samplesPerBit;
    samplesSinceMid++;
    if(mag>threshold){level=true;
    }else{level=false;}
    if(sampling==false){
        if(level){
  //          printf("\nStart sampling at %i \n", samplesSinceMid);
            samplesSinceMid=0;
            afterMid=level;
            beforeMid=false;
            bitEnd=samplesPerBit/2;
            sampling=true;
            nextGate=bitEnd+samplesPerBit/2-tol;
            bits=0;
            bytes=0;
        }
    }else{ //we are sampleing
        if(samplesSinceMid<bitEnd-tol){//covers from 0
            if(level==beforeMid){
            flush(1);
            }
        }
        if(samplesSinceMid==bitEnd-tol){// store before changing to the next bit
            if(beforeMid==false && afterMid==true){
                    newByte[bits]=true;
                    bits++;
            }else if(beforeMid==true && afterMid==false){
                    newByte[bits]=false;
                    bits++;
            }
            else{
                flush(2);
                }
            }
        if(samplesSinceMid==bitEnd+tol){
            beforeMid=level;
            }
        if(samplesSinceMid>bitEnd +tol && samplesSinceMid<nextGate){
            if(level==beforeMid){//we want if(level|=beforeMid) but not equal doesn't work for bool
            }else{
                flush(3);
                }
        }
        if(samplesSinceMid>nextGate && samplesSinceMid<nextGate+2*tol){
            if(level==beforeMid){
                gotChange=false;
            }else{
                afterMid=level;
                gotChange=true;
                samplesSinceMid=0;  //resyncs
                bitEnd=samplesPerBit/2;
                nextGate=bitEnd+samplesPerBit/2-tol;
                }
        }
        if(samplesSinceMid>=nextGate+2*tol && samplesSinceMid<samplesPerBit-tol){
            if(level==afterMid){
            }else{
            flush(4);
            }
            if(gotChange){
            }else{
            flush(5);
            }
        }
        if(samplesSinceMid>samplesPerBit+tol){
        flush(6);
        }
    if (bits>7){
        bits=0;
        bytes++;
        intWord=intWord<<8|bool2bin(newByte);
        if((intWord & 0xFF00000000000000)==0xFF00000000000000){//Only process it if we got a sync
            word2sensor(intWord);
            words++;
            bytes=0;
            sampling=false;
//             printf("Word= %" PRIX64,intWord);
//             printf("\n");
            intWord=0;
            }
     }
    }
}
      void flush(int type){
      printf("F %i  ",type);
      sampling=false;
      samplesSinceMid=0;
      }
