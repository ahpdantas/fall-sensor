/*
 * amost.c
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */
#include "fallsensor.h"
#include "fatfs/ff.h"
#include "utils/uartstdio.h"
#include "file.h"


FRESULT initFS(FATFS* fs);
void FSHandler(FallSensorDef *Fall);

FRESULT initFS(FATFS* fs)
{
	return f_mount(fs, "", 1);
}

void FSHandler(FallSensorDef *Fall)
{
	static FD_FILE File = NULL;
	static UINT fsize = 0;
	FRESULT result;
	UINT bw = 0;

	if( !Fall->flgs.IsReadyToSave )
		return;

	Fall->flgs.IsReadyToSave = 0;
	switch( Fall->state.save )
	{
		case OPEN_FILE:
			result = createFile(&File);
			if( result == FR_OK )
			{
				Fall->state.save = WRITE_FILE;
			}
			else
			{
				break;
			}
		case WRITE_FILE:
			result = writeFile(File,Fall->amost.buff.values[!Fall->amost.flgs.Active],Fall->amost.buff.size,&bw);
			if( result == FR_OK	)
			{
				fsize += bw;
				if( fsize == FILE_SIZE )
				{
					Fall->state.save = OPEN_FILE;
					result = closeFile(&File);
				}
			}
			break;
	}
}


