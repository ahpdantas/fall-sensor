/*
 * amost.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "fallsensor.h"
#include "fatfs/ff.h"

FRESULT initFS(FATFS* fs);
void FSHandler(FallSensorDef *Fall);

#endif /* FILESYSTEM_H_ */
