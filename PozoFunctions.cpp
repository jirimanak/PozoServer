#include <EEPROM.h>
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

//
// store IP address to EEPROM
// returns result  0 - OK,  result > 0 not ok
//
int store_ip_address(uint8_t *ip )
{
  EEPROM.write(1, ip[0]);
  EEPROM.write(2, ip[1]);
  EEPROM.write(3, ip[2]);
  EEPROM.write(4, ip[3]);
  EEPROM.write(0, 1);  // ip address is valid

  int result = 1;
  // return 
  if ( EEPROM.read(1) == 1)  result = 1;
  if ( EEPROM.read(1) == ip[0]) result = 2;
  if ( EEPROM.read(2) == ip[1]) result = 3;
  if ( EEPROM.read(3) == ip[2]) result = 4;
  if ( EEPROM.read(4) == ip[3]) result = 5;  

  return result;
}


//
// read IP address from EEPROM
//
int recall_ip_address(uint8_t *ip){

  if ( EEPROM.read(1)== 0) return 0;  //
  ip[0] = EEPROM.read(1);
  ip[1] = EEPROM.read(2);
  ip[2] = EEPROM.read(3);
  ip[3] = EEPROM.read(4);

  return 1;

}


//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void store_int(int p_address, int p_value)
{
  unsigned char lowByte = ((p_value >> 0) & 0xFF);
  unsigned char highByte = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int recall_int(int p_address)
{
  unsigned char lowByte = EEPROM.read(p_address);
  unsigned char highByte = EEPROM.read(p_address + 1);

  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

