#ifndef POZOFUNCTIONS_H_
#define POZOFUNCTIONS_H_

#include <Time.h>

int free_ram (); 
char* add_code_and_long(char* buff, long code, long val);
char* add_code_and_time(char* buff, long code, time_t val);
char* add_code_and_double(char* buff, long code, double val);
char* add_code_and_string(char* buff, long code, char* val);
#endif /* POZOFUNCTIONS_H_ */
