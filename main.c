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
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "init.h"
#include "fatfs/ff.h"

 #define RED_LED   GPIO_PIN_1
 #define BLUE_LED  GPIO_PIN_2
 #define GREEN_LED GPIO_PIN_3

typedef enum
{
	GetSensorData,
	SaveData,
	SendData,
}FallSensorStates;


unsigned long soundValuesBuff1[BUFFER_SIZE];
unsigned long soundValuesBuff2[BUFFER_SIZE];

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
	static unsigned long savingDataCount = 0;
	unsigned long *buffToSave = NULL;

	re = f_open(&Fil, "datalog.txt", FA_WRITE | FA_OPEN_ALWAYS );
	if( re == FR_OK)
	{
		UINT bw = 0;
		if( buff == soundValuesBuff1 )
		{
			buffToSave = soundValuesBuff2;
		}
		else if( buff == soundValuesBuff2 )
		{
			buffToSave = soundValuesBuff1;
		}

		re = f_write(&Fil, buffToSave, sizeof(unsigned long)*BUFFER_SIZE, &bw);
		re = f_close(&Fil);
	}
	savingDataCount++;
	if( savingDataCount == SAVING_DATA_COUNT )
	{
		State = SendData;
		savingDataCount = 0;
		GPIOPinWrite(GPIO_PORTF_BASE, RED_LED| BLUE_LED|GREEN_LED, GREEN_LED);
	}
	else
	{
		State = GetSensorData;
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
	init();

	buff = soundValuesBuff1;
	//save a file into SD Card to exemplify the library usage
	re = f_mount(&FatFs, "", 1);
	if( re == FR_OK )
	{
		UINT bw = 0;
		re = f_open(&Fil, "sound.txt", FA_WRITE | FA_CREATE_ALWAYS);
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
	}
}
