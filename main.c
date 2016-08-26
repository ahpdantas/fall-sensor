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



#define BUFFER_SIZE 1024

unsigned long soundValuesBuff1[BUFFER_SIZE];
unsigned long soundValuesBuff2[BUFFER_SIZE];

unsigned long *index = 0;
unsigned int transfers = 0;

FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */

void Adc0_1_ISR(void)
{
	// Clear the timer interrupt
	ADCIntClear(ADC0_BASE,1);

	ADCSequenceDataGet(ADC0_BASE, 1, &index[transfers]);
	transfers += 4;

	if( (transfers)%BUFFER_SIZE == 0 )
	{
		if( index == soundValuesBuff1 )
		{
			index = soundValuesBuff2;
		}
		else
		{
			index = soundValuesBuff1;
		}
		transfers = 0;
	}

	// Read the current state of the GPIO pin and
	// write back the opposite state
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
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

	FRESULT re;
	init();

	index = soundValuesBuff1;
	//save a file into SD Card to exemplify the library usage
	re = f_mount(&FatFs, "", 1);
	if( re == FR_OK )
	{
		UINT bw;
		re = f_open(&Fil, "sound.txt", FA_WRITE | FA_CREATE_ALWAYS);
		if( re == FR_OK)
		{
			f_write(&Fil, "It works fast now!\r\n", sizeof("It works fast now!\r\n"), &bw);
			f_close(&Fil);
		}
	}

   	while(1)
   	{
	}
}
