#include "PozoOneWire.h"
#include "PozoTypes.h"
#include "PozoCodes.h"
// #include <Serial.h>

PozoOneWire::PozoOneWire():ds(3){
  //byte data[12];
}

const char PozoOneWire::l[]= {
  "0123456789ABCDEF"};


int PozoOneWire::get_list(){

  Serial.println("Pozoz one wire");
  char buff[24];
  
  ds.reset_search();
  num_sensors = 0;
  while(ds.search(sensors[num_sensors]) ){
    if ( OneWire::crc8( sensors[num_sensors], 7) != sensors[num_sensors][7] ){
      Serial.println("Crc is not walid");   
      delay(250);
    }

 /*    
        if ( addr[0] != 0x10 && addr[0] != 0x28) {
      //Serial.print("Device is not recognized");
      continue;
    }
*/
    addr2str(sensors[num_sensors], buff);
    Serial.println(buff);   
    
    num_sensors++;  

  }

  return num_sensors;
}

//returns the temperature from one DS18S20 in DEG Celsius
// addr is sensor 1-wire address
float PozoOneWire::get_temp(byte addr[]){

  byte data[12];
  if (num_sensors < 0) return -1000.0;

  // Device is a DS18S20 family device && Device is a DS18B20 family device
  if ( (!addr[0] == 0x10) &&  (!addr[0] == 0x28)) {
    // Device is not a DS18S20 family device
    return -1000.0;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44);
  // delay(1000);

  byte present = ds.reset();
  ds.select(addr);  
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;

}

float PozoOneWire::get_temp(int idx){

  if (num_sensors <= 0) get_list(); 
  if ( idx >= num_sensors ) return -1000.0;
  float temp = get_temp( sensors[idx]);
  Serial.println(temp);
  return temp; 
}

// convert 1-wire address to printable string
// example:
// 0A:1B:2C:3D:4E:5F:61:72 char[24]
// 0A1B2C3D4E5F6172 char[17]
//
char* PozoOneWire::addr2str(byte addr[], char s[]){
  int j = 0;
  for (int i=0; i < 8; i++){
    s[j] = high2hex(addr[i]);
    j++;
    s[j] = low2hex(addr[i]); 
    j++;
/*
    if (i < 7){
      s[j] = ':';
      j++;
    }
*/
  }
  s[j] = '\0';
  return s;
}

char* PozoOneWire::get_addr(int num){
  if (num >= num_sensors)  return 0L;
  addr2str(sensors[num], buffer);
  Serial.println(buffer);
  return buffer;
}

char* PozoOneWire::str2addr(byte addr[], char s[]){
  
}


// conversion byte to HEX string format 
// s[3]

void PozoOneWire::byte2hex(byte i, char s[]){
  s[0] = high2hex(i);
  s[1] = low2hex(i);
  s[2] = '\0';
} 

// convert lower 4 bits
char PozoOneWire::low2hex(byte i){
  return l[int(i & B00001111)];
} 

// convert upper 4 bits
char PozoOneWire::high2hex(byte i){
  return l[int(i >> 4)];
} 

// s is input string  terminated by 0
char PozoOneWire::hex2byte(char s[], char buffer[]){
  char *h = s;
  char *b = buffer;
  
  for ( ; *h; h += 2, ++b) /* go by twos through the hex string */
   *b = ((strchr(l, *h) - l) * 16) /* multiply leading digit by 16 */
       + ((strchr(l, *(h+1)) - l));
  
} 
