/*
 * init.h
 *
 *  Created on: 26 de ago de 2016
 *      Author: andre
 */

#ifndef INCLUDE_INIT_H_
#define INCLUDE_INIT_H_

#define AMOST_FREQUENCY 4096
#define BUFFER_SIZE 1024
#define TIME_AQUISITION 16
#define SAVING_DATA_COUNT TIME_AQUISITION *( AMOST_FREQUENCY/BUFFER_SIZE )

void init();

#endif /* INCLUDE_INIT_H_ */
