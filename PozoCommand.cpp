#include "PozoCommand.h"
#include "PozoCodes.h"
#include "PozoTypes.h"
#include "PozoFunctions.h"
#include <arduino.h>

#include <string.h>
#include <Time.h>

extern time_t start_time;
extern int sversion;

PozoCommand::PozoCommand():
onewire()
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

  // set all realy pins to high = normal position (switched off)
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

  Serial.print("INPUT: ");
  Serial.println(str);

  //
  pch = strtok_r (str,":",&sp);

  t = now();

  // idx for value array  value[0] value[1] ...
  int i = 0;


  while (pch != NULL){

    // type is unknown - set it
    if (vtype == NOTYPE){
      vtype = pch[0];
      if (vtype == '\0') break;
    }
    else {
      // if type is already known parse value
      switch (vtype){
      case INTEGER:
      case LONG:
        Serial.print("LONG: ");
        if (i >= VALUE_MAXIDX) break;
        value[i].value_type = vtype;
        value[i].value.long_value = strtol(pch, NULL, 10);
        Serial.println(value[i].value.long_value);
        ++i;
        break;
      case FLOAT:
      case DOUBLE:
        Serial.print("DOUBLE: ");
        if (i >= VALUE_MAXIDX) break;
        value[i].value_type = vtype;
        value[i].value.double_value = strtod(pch, NULL);
        Serial.println(value[i].value.double_value);
        ++i;
        break;
      case TIMEDATE:
        Serial.print("TIMEDATE: ");
        if (i >= VALUE_MAXIDX) break;
        value[i].value_type = vtype;
        value[i].value.timedate_value = strtoul(pch, NULL,10);
        Serial.println(value[i].value.timedate_value);
        ++i;
        break;
      case ERRORCODE:
        Serial.print("ERRORCODE: ");
        errorcode = strtol(pch, NULL, 10);
        Serial.println(errorcode);
        break;
      case SENDER:
        Serial.print("SENDER: ");
        sender = strtol(pch, NULL, 10);                      
        Serial.println(sender);
        break;
      case COMMAND:
        command = strtol(pch, NULL, 10);
        Serial.print("Command: ");
        Serial.println(command);
        break;
      case BYTE:
        Serial.print("BYTE: ");
        if (i >= VALUE_MAXIDX) break;
        value[i].value_type = vtype;
        value[i].value.byte_value = (char)strtol(pch, NULL, 10);
        Serial.println(value[i].value.byte_value);
        ++i;
        break;
      case STRVALUE:
        Serial.print("STRING VALUE: ");
        if (i >= VALUE_MAXIDX) break;
        value[i].value_type = vtype;
        strcpy(value[i].value.str_value, pch);
        Serial.println(value[i].value.byte_value);
        ++i;
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


char* PozoCommand::execute(){
  char* ptr = response;
  int len;
  int running;
  float temp;
  time_t uptime;
  
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
      ptr = add_code_and_long(ptr, LONG, free_ram());
      break;
    case READHEADERVER:
      ptr = add_code_and_long(ptr, LONG, HEADERVERSION);
      break;
    case GETTIME:
      ptr = add_code_and_time(ptr, LONG, now());
      break;
    case PING:
      break;
    case SET_TIME:
      uptime = now() - start_time;
      setTime(value[0].value.timedate_value);
      start_time = now()- uptime;
      //start_time = now();
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
      errorcode = exe_setbinary(value[0].value.byte_value, value[1].value.long_value );
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
      temp = exe_read1wtemp((double)value[0].value.long_value-1);
      if ( temp <= -1000.0) { errorcode = OUTOFRANGE;}
      ptr = add_code_and_double(ptr, DOUBLE, temp);
      break;  
    case PINSTATUS:
      ptr = add_code_and_long(ptr, LONG, exe_pinstatus_pins());
      ptr = add_code_and_long(ptr, LONG, exe_pinstatus_time());
      break;  
    case UPTIME:
      uptime = now() - start_time;
      Serial.println(uptime);
      ptr = add_code_and_long(ptr, LONG, uptime);
      break;  
    case SVERSION:
      ptr = add_code_and_long(ptr, LONG, sversion);
      break;  

    default:
      errorcode = UNKNWNCMD;
      break;
    }

    ptr = add_code_and_long(ptr, ERRORCODE, errorcode);   
  }

  ptr[0] = '\0';
  return response;
}


int PozoCommand::check_time(time_t acttm) {
  for (int i = 0; i<8;i++){
    if (pin[i].end_time != 0){
      if (pin[i].end_time < acttm){
        if (pin[i].value == 0){
          digitalWrite(pin[i].ardu_pin, HIGH);
          pin[i].value = 1;
          pin[i].end_time = 0;
        }
      }
    }
  }
}


int PozoCommand::exe_onewire_list(){
  return onewire.get_list();
}

char* PozoCommand::exe_onewire_addr(int num){
  return onewire.get_addr(num);
}

int PozoCommand::exe_pinstatus_pins(){ 
    int result = 0, pow = 1;
    for ( int i = 7; i >= 0; --i, pow <<= 1 )
        result += pin[i].value * pow;
    Serial.println(result);    
    return result;
}

int PozoCommand::exe_pinstatus_time(){
  int maxtime = 0;
  for (int i = 0; i < 8; i++) {
     maxtime = max(maxtime, pin[i].end_time);
  } 
  if (maxtime != 0 )
     return pin[0].end_time - now();
  else 
     return 0;
}

float PozoCommand::exe_read1wtemp(int sensor){
  float result = OUTOFRANGE;
  if (!((sensor < 0) || (sensor > 7)))
     result = onewire.get_temp(sensor);
  return result;
}

int PozoCommand::exe_sethigh(int pinnum){
  if (pinnum < 0) return OUTOFRANGE;
  if (pinnum > 7) return OUTOFRANGE;
  digitalWrite(pin[pinnum].ardu_pin, HIGH);
  pin[pinnum].value = 1;
  pin[pinnum].end_time = 0;
  return OK;
}

int PozoCommand::exe_setlow(int pinnum, time_t period){
  if (pinnum < 0) return OUTOFRANGE;
  if (pinnum > 7) return OUTOFRANGE;
  digitalWrite(pin[pinnum].ardu_pin, LOW);
  pin[pinnum].value = 0;
  if (period == 0) {
     pin[pinnum].end_time = 0;
  }
  else {
    pin[pinnum].end_time = now() + period;
  }
  return OK;
}


int PozoCommand::exe_setbinary(char values, time_t period){
  int i;
  for ( i = 0; i<8; i++) {
    // test the bit on the positon 'i'
    if ( (values & (1<<(i))) ){
      // logic 1 = high
      exe_sethigh(7-i);
    }
    else {
      // logic 0 = low
      exe_setlow(7-i, period);
    }
  }
  return OK;

}
