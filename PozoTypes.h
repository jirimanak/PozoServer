/*
 * PozoTypes.h
 *
 *  Created on: 4.6.2014
 *      Author: A417280
 */

#ifndef POZOTYPES_H_
#define POZOTYPES_H_

#include "stdlib.h"
#include <Time.h>

#define PCSTR_LENMAX 32

// container for argument comming with POZO command to store value of any type
typedef struct _CmdValue {
	char  value_type;
	union {
	  long    long_value;   // L, I   (long, integer)
	  double  double_value; // D, F (double float, float)
	  time_t  timedate_value;   // T (time in seconds from the e
          char  byte_value; // B byte value - 8 bits
	  char str_value[PCSTR_LENMAX+1];  // S
	} value;
} CmdValue;


typedef struct _PinMap {
  int ardu_pin;  // maping to the aruduino physical pin
  int value; // current value
  unsigned long end_time;  // end time of the period when to change the value
} PinMap;

#endif /* POZOTYPES_H_ */

