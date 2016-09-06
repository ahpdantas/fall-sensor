/*
 * file.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef FILE_H_
#define FILE_H_

#include "fatfs/ff.h"

typedef struct FILE_DEF* FD_FILE;

FRESULT createFile(FD_FILE* File);
FRESULT writeFile(FD_FILE File, void* buff, UINT btw, UINT *bw);
FRESULT closeFile(FD_FILE* File);


#endif /* FILE_H_ */
