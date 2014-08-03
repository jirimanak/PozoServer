#include "PozoCommand.h"
#include "PozoCodes.h"
#include "PozoTypes.h"
#include "PozoFunctions.h"
#include <arduino.h>

#include <string.h>
#include <Time.h>

PozoCommand::PozoCommand():onewire()
{ 
   // mappin real pins to virtual 8 pins
  pin[0].ardu_pin = 6;  
  pin[1].ardu_pin = 7;
  pin[2].ardu_pin = 8;
  pin[3].ardu_pin = 9;
  pin[4].ardu_pin = 14; // VALVE A
  pin[5].ardu_pin = 15; // VALVE B
  pin[6].ardu_pin = 16; // VALVE C
  pin[7].ardu_pin = 17; // PUMP
  
  // set all realy pins to high = normal position (swithed off)
  // set time counter to zero
  // if zero no time period is set
  for (int i = 0; i < 8 ; i++){
    digitalWrite(pin[i].ardu_pin, HIGH);
    pin[i].value = 1;
    pin[i].end_time = 0;
  }
  
  reset();
}

void PozoCommand::reset(){
  sender = OTHER;
  command = NOPE;
  value[0].value_type = LONG;
  value[0].value.long_value = 0;
  value[1].value_type = LONG;
  value[1].value.long_value = 0;
  errorcode = OK;   	// if not 0 errorcode
  t = 0;   
}

void PozoCommand::copy_from(PozoCommand *pc){
  sender = pc->sender;
  command = pc->command;
  errorcode = pc->errorcode;
  t = pc->t;
  int i = 0;
  for (i = 0; i < VALUE_MAXIDX; i++){
    memcpy(&value[i],&pc->value[i], sizeof(pc->value[i]));
  }
}

int PozoCommand::set_value_double(double num, int idx) {
  if (idx >= VALUE_MAXIDX)
    return OUTOFRANGE;

  value[idx].value_type = DOUBLE;
  value[idx].value.long_value = num;

  return OK;  

}

int PozoCommand::set_value_long(long num, int idx) {
  if (idx >= VALUE_MAXIDX) 
    return OUTOFRANGE;

  value[idx].value_type = LONG;
  value[idx].value.long_value = num;

  return OK;  
}


int PozoCommand::set_value_timedate(time_t num, int idx) {
  if (idx >= VALUE_MAXIDX)
    return OUTOFRANGE;

  value[idx].value_type = TIMEDATE;
  value[idx].value.long_value = num;

  return OK;
}


int PozoCommand::parse(char* str) {

  //  S[ss]C[ccc]E[eee]D[dddd.dd]L[llllll]

  char *pch;
  char vtype = NOTYPE;
  char *sp;

  pch = strtok_r (str,":",&sp);

  t = now();

  // idx for value array  value[0] value[1] ...
  int i = 0;

  while (pch != NULL){

    // type is unknown - set it
    if (vtype == NOTYPE){
      vtype = pch[0];
    }
    else {
      // if type is already known parse value
      switch (vtype){
      case INTEGER:
      case LONG:
        if (i >= VALUE_MAXIDX) break;
        value[i].value_type = vtype;
        value[i].value.long_value = strtol(pch, NULL, 10);
        ++i;
        break;
      case FLOAT:
      case DOUBLE:
        if (i >= VALUE_MAXIDX) break;
        value[i].value_type = vtype;
        value[i].value.double_value = strtod(pch, NULL);
        ++i;
        break;
      case TIMEDATE:
        if (i >= VALUE_MAXIDX) break;
        value[i].value_type = vtype;
        value[i].value.timedate_value = strtoul(pch, NULL,10);
        ++i;
        break;
      case ERRORCODE:
        errorcode = strtol(pch, NULL, 10);
        break;
      case SENDER:
        sender = strtol(pch, NULL, 10);                      
        break;
      case COMMAND:
        command = strtol(pch, NULL, 10);
        break;  
      default:
        errorcode = BADTYPE;
        return errorcode;
      }
      vtype = NOTYPE;
    }
    pch = strtok_r (NULL,":",&sp);
  }

  return OK;

}

char* PozoCommand::print_all(){
      char* ptr = response;
      ptr = add_code_and_time(ptr, TIMEDATE, t);    
      ptr = add_code_and_long(ptr, ERRORCODE, errorcode);   
      ptr = add_code_and_long(ptr, SENDER, sender);
      ptr = add_code_and_long(ptr, COMMAND, command);
      return response;
}

int PozoCommand::exe_sethigh(int pinnum){
     if (pinnum < 0) return OUTOFRANGE;
     if (pinnum > 7) return OUTOFRANGE;
     digitalWrite(pin[pinnum].ardu_pin, HIGH);
     pin[pinnum].value = 1;
     return OK;
}
 
int PozoCommand::exe_setlow(int pinnum){
     if (pinnum < 0) return OUTOFRANGE;
     if (pinnum > 7) return OUTOFRANGE;
    digitalWrite(pin[pinnum].ardu_pin, LOW);
    pin[pinnum].value = 0;
    return OK;
}


int PozoCommand::exe_setbinary(char values){
  int i;
  for ( i = 0; i<8; i++) {
    // test the bit on the positon 'i'
    if ( (values & (1<<(i))) ){
      // logic 1 = high
      exe_sethigh(7-i);
    }
    else {
      // logic 0 = low
      exe_setlow(7-i);
    }
  }
  return OK;

}

float PozoCommand::exe_read1wtemp(int sensor){
    if (sensor < 0) return OUTOFRANGE;
    if (sensor > 7) return OUTOFRANGE;

    return onewire.get_temp(sensor);
}

char* PozoCommand::execute(){

  char* ptr = response;
  int len;
  
  if (errorcode != OK) 
  {
    // sprintf(response,"%c[%l]",ERRORCODE,errorcode);
    ptr = add_code_and_long(ptr, ERRORCODE, errorcode);
    ptr = add_code_and_time(ptr, TIMEDATE, t);
  }
  else {
      ptr = add_code_and_time(ptr, TIMEDATE, now());    
      ptr = add_code_and_long(ptr, COMMAND, command);
    switch ( command ){
    case FREERAM:
      ptr = add_code_and_time(ptr, LONG, free_ram());
      break;
    case GETTIME:
      ptr = add_code_and_time(ptr, LONG, now());
      break;
    case PING:
      break;
    case SET_TIME:
      setTime(value[0].value.timedate_value);
      ptr = add_code_and_time(ptr, TIMEDATE, now()); 
      break;
    case NOPE:
      break;
    case SETHIGH:
      errorcode = exe_sethigh(value[0].value.long_value-1);
      break;   
    case SETLOW:
      errorcode = exe_setlow(value[0].value.long_value-1);
      break;
    case SETBINARY:
      errorcode = exe_setbinary(value[0].value.byte_value);
      break;
    case GET1WNUM:
      ptr = add_code_and_long(ptr, LONG, exe_onewire_list());
      break;  
    case READ1WNUM:
      break;  
    case READ1WADDR:
      ptr = add_code_and_string(ptr, STRVALUE, exe_onewire_addr(value[0].value.long_value-1) );
      break;  
    case READ1WTEMP:
      ptr = add_code_and_double(ptr, DOUBLE, exe_read1wtemp((double)value[0].value.long_value-1));
      break;  
    default:
      break;
    }
    
    ptr = add_code_and_long(ptr, ERRORCODE, errorcode);   
  }

  ptr[0] = '\0';
  return response;
}

int PozoCommand::exe_onewire_list(){
  return onewire.get_list();
}

char* PozoCommand::exe_onewire_addr(int num){
  return onewire.get_addr(num);
}

