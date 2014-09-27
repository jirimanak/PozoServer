#ifndef POZOONEWIRE_H_
#define POZOONEWIRE_H_

#include <OneWire.h>

class PozoOneWire
{
  private:
  OneWire ds;
  int num_sensors;
  byte sensors[10][8];
  const static char l[];
  char buffer[24]; // string output buffer
  
  public:
  PozoOneWire();
  float get_temp(byte addr[]);
  float get_temp(int num);
  int get_list();
  char* get_addr(int num);
  
  char* addr2str(byte addr[], char s[]);
  char* str2addr(byte addr[], char s[]);
  void byte2hex(byte i, char s[]);
  char low2hex(byte i);
  char high2hex(byte i);
  char hex2byte(char s[], char buffer[]);
};

#endif /* POZOONEWIRE_H_ */

