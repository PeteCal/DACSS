#include "x345MHz.h"
#include <mosquitto.h>


void StartUp(){
    getCoeficients();
    getMessageDatabase();
    createMosquitto();
}


