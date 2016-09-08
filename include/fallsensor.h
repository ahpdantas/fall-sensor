/*
 * fallsensor.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef FALLSENSOR_H_
#define FALLSENSOR_H_

#include "../filemanager.h"
#include "../esp8266manager.h"
#include "config.h"

typedef struct{
	struct{
		unsigned short _0[AMOST_BUFFER_SIZE];
		unsigned short _1[AMOST_BUFFER_SIZE];
		unsigned short *active;
		unsigned short *toSave;
		unsigned int size;
	}buff;
	struct{
		unsigned int Active:1;
	}flgs;

}BUFFER_DEF;

typedef struct{
	BUFFER_DEF amost;
	struct{
		unsigned int IsReadyToSave:1;
	}flgs;
}FALL_SENSOR_DEF;


#endif /* FALLSENSOR_H_ */
