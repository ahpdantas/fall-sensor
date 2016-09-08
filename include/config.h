/*
 * config.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef INCLUDE_CONFIG_H_
#define INCLUDE_CONFIG_H_

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

#define AMOST_FREQUENCY 16
#define AMOST_BUFFER_SIZE 16
#define NUMBER_OF_SAMPLES 4
#define TIME_AQUISITION 10

#define NUMBER_OF_WRITE_CYCLES (TIME_AQUISITION *( AMOST_FREQUENCY/(AMOST_BUFFER_SIZE/NUMBER_OF_SAMPLES) ))




#endif /* INCLUDE_CONFIG_H_ */
