#ifndef POZOCOMMAND_H_
#define POZOCOMMAND_H_

#include "PozoTypes.h"
#include "PozoOneWire.h"

#define VALUE_MAXIDX 2
#define SENDER_ID_LEN 2
#define RESPONSE_STR_MAXSIZE 100



class PozoCommand
{
  public:
  long sender;   	           // who send this command - initator
  long command;     	           // command code
  long errorcode;   	           // if not 0 errorcode
  CmdValue value[VALUE_MAXIDX];    // value
  time_t t;                        // system time when command recieved
  PinMap pin[8];
  PozoOneWire onewire;
  
  char response[RESPONSE_STR_MAXSIZE]; // buffer for the response string
  
  public:
  PozoCommand();
  void reset();
  int parse(char* str);
  int set_value_double(double num, int idx);
  int set_value_long(long num, int idx);
  int set_value_timedate(time_t num, int idx);
  void copy_from(PozoCommand *pc);
  char* execute();
  char* print_all();

  float exe_read1wtemp(int sensor);
  int exe_sethigh(int pinnum);
  int exe_setlow(int pinnum);
  int exe_setbinary(char pinnum);
  private:
  int parse_value(char* str);
  
  int exe_onewire_list();
  char* exe_onewire_addr(int num);

};

#endif /* POZOCOMMAND_H_ */
