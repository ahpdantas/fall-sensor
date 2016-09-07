/*
 * amost.c
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */
#include "file.h"

#include "fallsensor.h"
#include "fatfs/ff.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "queue.h"
#include "config.h"

#define NUMBER_OF_WRITE_CYCLES (TIME_AQUISITION *( AMOST_FREQUENCY/AMOST_BUFFER_SIZE ))
#define FILENAME_SIZE 16
#define FILE_ID_MAX 9999

FRESULT initVolume(FD_FAT_FS_DEF* FdFS)
{
	FRESULT result;

	result = f_mount( &FdFS->FatFs, "", 1);
	if( result == FR_OK )
	{
		FdFS->FilesQueue = CreateQueue();
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
		UARTprintf(" %s openned with sucess.\n", filename );
		result = f_write(File, filename, FILENAME_SIZE,&bw);
	}

	return result;
}

void FileSaveHandler(FALL_SENSOR_DEF *Fall)
{
	static FIL File;
	static unsigned int FileId = 0;
	static unsigned int numberOfWrites = 0;
	FRESULT result;
	UINT bw = 0;

	if( !Fall->flgs.IsReadyToSave )
		return;

	Fall->flgs.IsReadyToSave = 0;

	switch( Fall->state.save )
	{
		case OPEN_FILE:
			result = createFile(&File, &FileId);
			if( result == FR_OK )
			{
				Fall->state.save = WRITE_FILE;
			}
			else
			{
				UARTprintf("ERROR in createFile() - ERROR: %d\n", result);
				break;
			}
		case WRITE_FILE:
			result = f_write( &File, Fall->amost.buff.toSave, Fall->amost.buff.size, &bw);
			if( result == FR_OK	)
			{
				numberOfWrites++;
				if( numberOfWrites == NUMBER_OF_WRITE_CYCLES )
				{
					Fall->state.save = OPEN_FILE;
					numberOfWrites = 0;
					result = f_close(&File);
					if( result == FR_OK )
					{
						Enqueue( Fall->FdFs.FilesQueue, FileId );
					}
				}
			}
			break;
	}
}


