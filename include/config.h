/*
 * config.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef INCLUDE_CONFIG_H_
#define INCLUDE_CONFIG_H_

#define AMOST_FREQUENCY 20000
#define AMOST_BUFFER_SIZE 2048
#define TIME_AQUISITION 32
#define TIME_TIMER1A_INTERRUPT 100 //ms
#define FREQ_TIMER1_INTERRUPT (1000/TIME_TIMER1A_INTERRUPT)
#define FILE_SIZE (TIME_AQUISITION *( AMOST_FREQUENCY/AMOST_BUFFER_SIZE ))


#endif /* INCLUDE_CONFIG_H_ */
