/*
 * file.c
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */
#include <stdlib.h>
#include "fatfs/ff.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "file.h"

#define FILENAME_SIZE 16

typedef struct
{
	char name[FILENAME_SIZE];
	unsigned int size;
}FILE_HEADER_DEF;

typedef struct
{
	FIL fil;
	FILE_HEADER_DEF att;
}FILE_DEF;

void getFilename(char* fileDst, unsigned int size )
{
	static unsigned int fileID = 0;

	usnprintf(fileDst,size,"S%05d.txt",fileID);
	fileID++;
}

FRESULT createFile(FD_FILE* File)
{
	FRESULT result;
	UINT bw;

	if(*File == NULL)
	{
		*File = malloc(sizeof(FILE_DEF));
	}

	getFilename( ((FILE_DEF *)*File)->att.name, FILENAME_SIZE);
	result = f_open(&((FILE_DEF *)*File)->fil, ((FILE_DEF *)*File)->att.name, FA_WRITE | FA_OPEN_ALWAYS );
	if( result == FR_OK )
	{
		result = f_write(&((FILE_DEF *)*File)->fil, &((FILE_DEF *)*File)->att, sizeof(FILE_HEADER_DEF),&bw);
	}

	return result;
}

FRESULT writeFile(FD_FILE File, void* buff, UINT btw, UINT *bw)
{
	return f_write( &((FILE_DEF*)File)->fil, buff, btw, bw);
}

FRESULT closeFile(FD_FILE* File)
{
	FRESULT result;
	result = f_close(&((FILE_DEF *)*File)->fil);
	if( *File != NULL )
	{
		free(*File);
		*File = NULL;
	}

	return result;
}
