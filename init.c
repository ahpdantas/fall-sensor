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
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "config.h"
#include "fallsensor.h"
#include "rtc.h"

#include "esp8266manager.h"
#include "filemanager.h"

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
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH1);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);
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


void initUART1()
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH2_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH2_GPIOB);

	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 );

	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE );
	UARTStdioInitExpClk(1,115200);
}

void init(FALL_SENSOR_DEF* Fall)
{
	//configure the system clock to run at 40MHz
	Fall->amost.flgs.Active = 0;
	Fall->amost.buff.active = Fall->amost.buff._0;
	Fall->amost.buff.toSave = Fall->amost.buff._1;
	Fall->amost.buff.size = sizeof(*Fall->amost.buff._0)*AMOST_BUFFER_SIZE;
	Fall->flgs.IsReadyToSave = 0;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	initGpios();
	initADC0();
	initTimer0();
	initTimer1();
	initRTC();
	initUART1();
	initVolume();
	initESP8266();

}

