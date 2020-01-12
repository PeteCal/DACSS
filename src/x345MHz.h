#ifndef X345MHZ_H_INCLUDED
#define X345MHZ_H_INCLUDED

 void StartUp();
 void processSample(unsigned char,unsigned char,float *filteredI,float *filteredQ);
 void mag2word(unsigned int);
 int getCoeficients();
 void word2sensor(unsigned long long int intWord);
 void add2log(char *messageToLog ,int value);
 void publish(char *Topic, char *action);
 void getMessageDatabase();
 void createMosquitto();
// .h file name can't start with a digit.

#endif // X345MHZ_H_INCLUDED
