/*
 * init.h
 *
 *  Created on: 26 de ago de 2016
 *      Author: andre
 */

#ifndef INCLUDE_INIT_H_
#define INCLUDE_INIT_H_

#include "esp8266/esp8266.h"

#define AMOST_FREQUENCY 4096
#define BUFFER_SIZE 1024
#define TIME_AQUISITION 16
#define TIME_TIMER1A_INTERRUPT 100 //ms
#define FREQ_TIMER1_INTERRUPT (1000/TIME_TIMER1A_INTERRUPT)
#define SAVING_DATA_COUNT (TIME_AQUISITION *( AMOST_FREQUENCY/BUFFER_SIZE ))

extern ESP8266_t ESP8266;

void init();

#endif /* INCLUDE_INIT_H_ */
