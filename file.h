/*
 * amost.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef FILE_H_
#define FILE_H_

#include "fallsensor.h"

FRESULT initVolume(FD_FAT_FS_DEF* FdFS);
void FileSaveHandler(FALL_SENSOR_DEF *Fall);

#endif /* FILE_H_ */
