#include "PozoFunctions.h"

#include <stdlib.h>
#include <string.h>

int free_ram () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


char* add_code_and_long(char* buff, long code, long val){
    int len;
    
    buff[0] = code;
    buff[1] = ':';
    ltoa(val, &buff[2], 10);
    len = strlen(buff);
    buff[len]=':';
    buff[len+1]='\0'; 
  return &buff[len+1];
}

char* add_code_and_time(char* buff, long code, time_t val){

    int len;
  
    buff[0] = code;
    buff[1] = ':';
    ltoa(val, &buff[2], 10);
    len = strlen(buff);
    buff[len]=':';
    buff[len+1]='\0'; 
  return &buff[len+1];
}


char* add_code_and_double(char* buff, long code, double val){

    int len;
  
    buff[0] = code;
    buff[1] = ':';
    dtostrf(val, 2, 2, &buff[2]);
    len = strlen(buff);
    buff[len]=':';
    buff[len+1]='\0'; 
  return &buff[len+1];
}

char* add_code_and_string(char* buff, long code, char* val){

    int len;
  
    buff[0] = code;
    buff[1] = ':';
    buff[2] = '"';
    strcpy(&buff[3], val);
    len = strlen(buff);
    buff[len]='"';
    buff[len+1]=':';
    buff[len+2]='\0'; 
  return &buff[len+2];
}



