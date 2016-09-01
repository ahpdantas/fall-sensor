#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "utils/ustdlib.h"
#include "init.h"
#include "fatfs/ff.h"

 #define RED_LED   GPIO_PIN_1
 #define BLUE_LED  GPIO_PIN_2
 #define GREEN_LED GPIO_PIN_3

#define FILE_NAME_BUFFER_SIZE  32

typedef enum
{
	GetSensorData,
	SaveData,
	SendData,
}FallSensorStates;

typedef enum
{
	CREATE_OPEN_NEW_FILE,
	CLOSE_SEND_FILE = SAVING_DATA_COUNT
}SavingState;

unsigned long soundValuesBuff1[BUFFER_SIZE];
unsigned long soundValuesBuff2[BUFFER_SIZE];

char FileName[FILE_NAME_BUFFER_SIZE];

unsigned long *buff = NULL;
unsigned int transfers = 0;

FallSensorStates State = GetSensorData;

FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */
FRESULT re;

void Adc0_1_ISR(void)
{
	// Clear the timer interrupt
	ADCIntClear(ADC0_BASE,1);

	ADCSequenceDataGet(ADC0_BASE, 1, &buff[transfers]);
	transfers += 4;

	if( (transfers)%BUFFER_SIZE == 0 )
	{
		if( buff == soundValuesBuff1 )
		{
			buff = soundValuesBuff2;
		}
		else
		{
			buff = soundValuesBuff1;
		}
		State = SaveData;
		transfers = 0;

		// Read the current state of the GPIO pin and
		// write back the opposite state
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) )
		{
			GPIOPinWrite(GPIO_PORTF_BASE, RED_LED| BLUE_LED|GREEN_LED, 0);
		}
		else
		{
			GPIOPinWrite(GPIO_PORTF_BASE, RED_LED| BLUE_LED|GREEN_LED, BLUE_LED);
		}
	}
}
void SaveSensorsData()
{
	static SavingState savingState = CREATE_OPEN_NEW_FILE;
	static unsigned long *buffToSave = NULL;
	static unsigned int fileID = 0;
	UINT bw = 0;

	switch( savingState )
	{
	case CREATE_OPEN_NEW_FILE:

		usnprintf(FileName,FILE_NAME_BUFFER_SIZE,"S%05d.txt",fileID);
		re = f_open(&Fil, FileName, FA_WRITE | FA_OPEN_ALWAYS );
		if( re == FR_OK )
		{
			savingState++;
			fileID++;
			if( buff == soundValuesBuff1 )
			{
				buffToSave = soundValuesBuff2;
			}
			else if( buff == soundValuesBuff2 )
			{
				buffToSave = soundValuesBuff1;
			}
		}
		break;
	case CLOSE_SEND_FILE:
		re = f_close(&Fil);
		if( re == FR_OK )
		{
			State = SendData;
			savingState = CREATE_OPEN_NEW_FILE;
			GPIOPinWrite(GPIO_PORTF_BASE, RED_LED| BLUE_LED|GREEN_LED, GREEN_LED);
		}
		break;
	default:
		re = f_write(&Fil, buffToSave, sizeof(unsigned long)*BUFFER_SIZE, &bw);
		if( re != FR_OK )
		{
			GPIOPinWrite(GPIO_PORTF_BASE, RED_LED| BLUE_LED|GREEN_LED, RED_LED);
		}
		savingState++;
		State = GetSensorData;
		break;

	}
}

/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */

int main(void)
{
	char data;
	init();

	buff = soundValuesBuff1;
	//save a file into SD Card to exemplify the library usage
	re = f_mount(&FatFs, "", 1);
	if( re == FR_OK )
	{
		UINT bw = 0;
		re = f_open(&Fil, "sdtest.txt", FA_WRITE | FA_OPEN_ALWAYS );
		if( re == FR_OK)
		{
			f_write(&Fil, "It works fast now!\r\n", sizeof("It works fast now!\r\n"), &bw);
			f_close(&Fil);
		}
	}

   	while(1)
   	{
   		switch(State)
   		{
   		case GetSensorData:
   			break;
   		case SaveData:
   			SaveSensorsData();
   			break;
   		case SendData:
   			State = GetSensorData;
   			break;
   		}
/*
   		//Test WIFI Module
		if(UARTCharsAvail(UART3_BASE))
		{
			UARTCharPut(UART1_BASE, UARTCharGet(UART3_BASE) );
		}

		if(UARTCharsAvail(UART1_BASE))
		{
			UARTCharPut(UART3_BASE, UARTCharGet(UART1_BASE) );
		}*/


	}
}
