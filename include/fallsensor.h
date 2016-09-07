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
#include "../queue.h"


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

typedef struct
{
	FATFS FatFs;
	QUEUE FilesQueue;
}FD_FAT_FS_DEF;

typedef struct{
	struct{
		unsigned long _0[AMOST_BUFFER_SIZE];
		unsigned long _1[AMOST_BUFFER_SIZE];
		unsigned long *active;
		unsigned long *toSave;
		unsigned int size;
	}buff;
	struct{
		unsigned int Active:1;
	}flgs;

}BUFFER_DEF;

typedef struct{
	FD_FAT_FS_DEF FdFs;		/* FatFs work area needed for each volume */
	ESP8266_t ESP8266;
	struct{
		FileState save;
		FileState read;
		ConnState send;
	}state;
	BUFFER_DEF amost;
	struct{
		unsigned int IsReadyToSave:1;
	}flgs;
}FALL_SENSOR_DEF;


#endif /* FALLSENSOR_H_ */
