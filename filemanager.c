/*
 * amost.c
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#include "filemanager.h"
#include <stdlib.h>
#include "fatfs/ff.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "queue.h"
#include "config.h"

#define FILENAME_SIZE 16
#define FILE_ID_MAX 9999

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
	struct{
		FileState write;
		FileState read;
	}state;
}FILE_MANAGER_DEF;


int initVolume(FILE_MANAGER *FileManager )
{
	FILE_MANAGER_DEF *fm;
	FRESULT result;

	fm = malloc(sizeof(FILE_MANAGER_DEF));

	fm->state.read = OPEN_FILE;
	fm->state.write = OPEN_FILE;

	result = f_mount( &fm->FatFs, "", 1);
	if( result == FR_OK )
	{
		fm->FilesQueue = CreateQueue();
		*FileManager = (FILE_MANAGER)fm;
	}

	return result;
}

void getFilenameById(char* fileDst, unsigned int size, unsigned int Id )
{
	usnprintf(fileDst,size,"FD%04d.txt",Id);
}

void createNewFilename(char* fileDst, unsigned int size, UINT *Id )
{
	*Id += 1;
	if( *Id > FILE_ID_MAX )
	{
		*Id = 0;
	}

	getFilenameById(fileDst,size,*Id);
}

FRESULT openFile(FIL *File, UINT Id)
{
	FRESULT result;
	CHAR filename[FILENAME_SIZE];

	getFilenameById(filename,FILENAME_SIZE,Id);
	result = f_open( File, filename, FA_READ | FA_OPEN_EXISTING );
	return result;
}

FRESULT createFile(FIL* File, UINT* Id )
{
	FRESULT result;
	CHAR filename[FILENAME_SIZE];
	UINT bw;

	createNewFilename(filename, FILENAME_SIZE, Id);
	result = f_open( File, filename, FA_WRITE | FA_OPEN_ALWAYS );
	if( result == FR_OK )
	{
		result = f_write(File, filename, FILENAME_SIZE,&bw);
	}
	return result;
}

int FileReadHandler(FILE_MANAGER FileManager, char* buffer, unsigned int bufferSize)
{
	static FIL File;
	static unsigned int FileId = 0;
	FILE_MANAGER_DEF *fm = NULL;
	FRESULT result;
	UINT br = 0;

	fm = (FILE_MANAGER_DEF*)FileManager;

	if( IsEmpty( fm->FilesQueue ) ) return 0;

	switch( fm->state.read )
	{
		case OPEN_FILE:
			Dequeue(fm->FilesQueue, &FileId);
			result = openFile(&File, FileId);
			if( result == FR_OK )
			{
				UARTprintf("File %d oppened with sucess.\n", FileId );
				UARTprintf("writing...\n");
				fm->state.read = READ_FILE;
			}
			else
			{
				UARTprintf("ERROR in openFile() - ERROR: %d\n", result);
				break;
			}
		case READ_FILE:
			result = f_read(&File,buffer,bufferSize,&br);
			if( result == FR_OK && br == 0 )
			{
				fm->state.read = OPEN_FILE;
				f_close(&File);
			}
			break;
	}
	return br;
}

void FileWriteHandler(FILE_MANAGER FileManager, char* buffer, unsigned int bufferSize )
{
	static FIL File;
	static unsigned int FileId = 0;
	static unsigned int numberOfWrites = 0;
	FILE_MANAGER_DEF *fm = NULL;
	FRESULT result;
	UINT bw = 0;

	fm = (FILE_MANAGER_DEF*)FileManager;

	switch( fm->state.write )
	{
		case OPEN_FILE:
			result = createFile(&File, &FileId);
			if( result == FR_OK )
			{
				UARTprintf("File %d oppened with sucess.\n", FileId );
				UARTprintf("writing...\n");
				fm->state.write = WRITE_FILE;
			}
			else
			{
				UARTprintf("ERROR in createFile() - ERROR: %d\n", result);
				break;
			}
		case WRITE_FILE:
			result = f_write( &File, buffer, bufferSize, &bw);
			if( result == FR_OK	)
			{
				numberOfWrites++;
				if( numberOfWrites == NUMBER_OF_WRITE_CYCLES )
				{
					fm->state.write = OPEN_FILE;
					numberOfWrites = 0;
					result = f_close(&File);
					if( result == FR_OK )
					{
						UARTprintf("File %d closed with sucess.\n", FileId );
						Enqueue( fm->FilesQueue, FileId );
					}
				}
			}
			break;
	}
}

