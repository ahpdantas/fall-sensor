/*
 * init.c
 *
 *  Created on: 26 de ago de 2016
 *      Author: andre
 */
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "init.h"
#include "rtc.h"

void initGpios()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

void initTimer0()
{
	unsigned long ulPeriod = 0;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	ulPeriod = ( SysCtlClockGet() / AMOST_FREQUENCY);

	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod -1);
	TimerControlTrigger(TIMER0_BASE,TIMER_A,true);
	TimerEnable(TIMER0_BASE, TIMER_A);
}

void initADC0()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);
	ADCSequenceDisable(ADC0_BASE, 1);

	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_TIMER, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);

	ADCIntEnable(ADC0_BASE,1);
	IntEnable(INT_ADC0SS1);

}

void initRTC()
{
	SysTickPeriodSet(10000000); //max 16mio
	SysTickIntRegister(RTCIntHandler);
	SysTickIntEnable();
	SysTickEnable();
}

void init()
{
	//configure the system clock to run at 40MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	initGpios();
	initADC0();
	initTimer0();
	initRTC();
}

