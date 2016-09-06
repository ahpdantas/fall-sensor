/*
 * fallsensor.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef FALLSENSOR_H_
#define FALLSENSOR_H_

#include "config.h"
#include "fatfs/ff.h"
#include "esp8266/esp8266.h"


typedef enum
{
	OPEN_CONNECTION,
	SEND_DATA,
	CLOSE_CONNECTION
}ConnState;

typedef enum
{
	OPEN_FILE,
	READ_FILE,
	WRITE_FILE,
	CLOSE_FILE
}FileState;

typedef struct{
	struct{
		unsigned long *values[2];
		unsigned int size;
	}buff;
	struct{
		unsigned int Active:1;
	}flgs;

}BuffersDef;

typedef struct{
	FATFS FatFs;		/* FatFs work area needed for each volume */
	ESP8266_t ESP8266;
	struct{
		FileState save;
		ConnState send;
	}state;
	BuffersDef amost;
	struct{
		unsigned int IsReadyToSave:1;
	}flgs;
}FallSensorDef;


#endif /* FALLSENSOR_H_ */
