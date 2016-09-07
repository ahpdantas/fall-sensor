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
#include "utils/uartstdio.h"
#include "fallsensor.h"
#include "init.h"

#include "esp8266manager.h"
#include "filemanager.h"

static FALL_SENSOR_DEF Fall;

void Adc0_1_ISR(void)
{
	static unsigned int transfers = 0;
	// Clear the timer interrupt
	ADCIntClear(ADC0_BASE,1);

	ADCSequenceDataGet(ADC0_BASE, 1, &Fall.amost.buff.active[transfers]);
	transfers += 4;
	if( (transfers)%AMOST_BUFFER_SIZE == 0 )
	{
		Fall.amost.flgs.Active ^= 1;
		if( Fall.amost.flgs.Active )
		{
			Fall.amost.buff.toSave = Fall.amost.buff._0;
			Fall.amost.buff.active = Fall.amost.buff._1;
		}
		else
		{
			Fall.amost.buff.toSave = Fall.amost.buff._1;
			Fall.amost.buff.active = Fall.amost.buff._0;
		}

		Fall.flgs.IsReadyToSave = 1;
		transfers = 0;
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
	init(&Fall);
	while(1)
   	{

		if( Fall.flgs.IsReadyToSave )
		{
			FileWriteHandler((char*)Fall.amost.buff.toSave, Fall.amost.buff.size);
			Fall.flgs.IsReadyToSave = 0;
		}
		ESP8266Handler();
	}
}

