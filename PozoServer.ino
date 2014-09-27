#include <Time.h>
#include "PozoCommand.h"
#include "PozoCodes.h"
#include "PozoTypes.h"
#include <string.h>
#include "ETHER_28J60.h"
#include <EtherShield.h>
#include "PozoFunctions.h"
#include <OneWire.h>


PozoCommand pc;

static uint8_t mac[6] = {
  0x54, 0x55, 0x58, 0x10, 0x00, 0x24};   // this just needs to be unique for your network, 

static uint8_t ip[4] = {
  192, 168, 0, 15}; // IP address for the webserver

static uint16_t port = 80; // Use port 80 - the standard for HTTP

ETHER_28J60 e;
char* params;

time_t start_time = 0;
float temp_eps = 0.1;  
int sversion = 10;


// digital pins
// D0 - RX  
// D1 - TX
// D2 - Connects the INT pin of the ENC28J60
// D3 – 1Wire 
// D4 – I2C SDA
// D5 – I2C SCL
// D6 - Relay 
// D7 - Relay
// D8 - Relay
// D9 - Relay
// D10 - Used for SPI Chip Select  
// D11 - Used for SPI MOSI
// D12 - Used for SPI MISO
// D13 - Used for SPI SCK
// D14(A0) – Relay
// D15(A1) – Relay
// D16(A2) – Relay
// D17(A3) - Relay
// D18(A4) - 1st presure sensor
// D19(A5) – 2nd presure sensor

void setup(){

  // set default time  
  setTime(0,0,0,1,1,2000);
 
  start_time = now();
  
  e.setup(mac, ip, port);

  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT); 

  pinMode(14, OUTPUT); 
  pinMode(15, OUTPUT); 
  pinMode(16, OUTPUT); 
  pinMode(17, OUTPUT); 

  // set all relays off
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  
  digitalWrite(14, HIGH);
  digitalWrite(15, HIGH);
  digitalWrite(16, HIGH);
  digitalWrite(17, HIGH);

  Serial.begin(9600);  
  delay(500);
  digitalWrite(16, 1);

}


int parse(char* str, PozoCommand *pc) {

  //  S[ss]C[ccc]E[eee]D[dddd.dd]L[llllll]

  char *pch;
  char vtype = NOTYPE;
  char *sp;

  Serial.print("INPUT: ");
  Serial.println(str);
  
  //
  pch = strtok_r (str,":",&sp);

  pc->t = now();

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
        pc->value[i].value_type = vtype;
        pc->value[i].value.long_value = strtol(pch, NULL, 10);
        Serial.println(pc->value[i].value.long_value);
        ++i;
        break;
      case FLOAT:
      case DOUBLE:
        Serial.print("DOUBLE: ");
        if (i >= VALUE_MAXIDX) break;
        pc->value[i].value_type = vtype;
        pc->value[i].value.double_value = strtod(pch, NULL);
        Serial.println(pc->value[i].value.double_value);
        ++i;
        break;
      case TIMEDATE:
        // time is in microseconds from 1.1.1970
        Serial.print("TIMEDATE: ");
        if (i >= VALUE_MAXIDX) break;
        pc->value[i].value_type = vtype;
        pc->value[i].value.timedate_value = strtoul(pch, NULL,10);
        Serial.println(pc->value[i].value.timedate_value);
        ++i;
        break;
      case ERRORCODE:
        Serial.print("ERRORCODE: ");
        pc->errorcode = strtol(pch, NULL, 10);
        Serial.println(pc->errorcode);
        break;
      case SENDER:
        Serial.print("SENDER: ");
        pc->sender = strtol(pch, NULL, 10);                      
        Serial.println(pc->sender);
        break;
      case COMMAND:
        pc->command = strtol(pch, NULL, 10);
        Serial.print("Command: ");
        Serial.println(pc->command);
        break;
     case BYTE:
        Serial.print("BYTE: ");
        if (i >= VALUE_MAXIDX) break;
        pc->value[i].value_type = vtype;
        pc->value[i].value.byte_value = (char)strtol(pch, NULL, 10);
        Serial.println(pc->value[i].value.byte_value);
        ++i;
        break;
     case STRVALUE:
        Serial.print("STRING VALUE: ");
        if (i >= VALUE_MAXIDX) break;
        pc->value[i].value_type = vtype;
        strcpy(pc->value[i].value.str_value, pch);
        Serial.println(pc->value[i].value.byte_value);
        ++i;
        break;

      default:
        pc->errorcode = BADTYPE;
        return pc->errorcode;
      }
      vtype = NOTYPE;
    }
    pch = strtok_r (NULL,":",&sp);
  }

  return OK;

}


void loop(){
  char *response;

  pc.check_time(now());
  
  if (params = e.serviceRequest())
  {
    pc.reset();
    Serial.print("params: ");
    Serial.println(params);

    pc.parse(params);
    response = pc.execute();
    Serial.print("RESPONSE: ");
    Serial.println(response);
    e.print("POZO/");
    e.print(response);
    e.respond();
  }  

}

