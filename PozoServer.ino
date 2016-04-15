#include <ETHER_28J60.h>
#include <enc28j60.h>
#include <ip_arp_udp_tcp.h>
#include <net.h>
#include <Time.h>
#include "PozoCommand.h"
#include "PozoCodes.h"
#include "PozoTypes.h"
#include <string.h>
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
time_t sversion = 501;


// digital pins
// D0 - RX  
// D1 - TX
// D2 - Connects the INT pin of the ENC28J60
// D3 - 1-Wire 
// D4 - I2C SDA
// D5 - I2C SCL
// D6 - Relay 
// D7 - Relay
// D8 - Relay
// D9 - Relay
// D10 - Used for SPI Chip Select  
// D11 - Used for SPI MOSI
// D12 - Used for SPI MISO
// D13 - Used for SPI SCK
// D14(A0) - Relay
// D15(A1) - Relay
// D16(A2) - Relay
// D17(A3) - Relay
// D18(A4) - 1st presure sensor
// D19(A5) - 2nd presure sensor

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


