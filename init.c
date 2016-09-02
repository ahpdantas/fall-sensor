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
#include "init.h"
#include "rtc.h"

char UARTRecBuffer[128];
ESP8266_t ESP8266;

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

void initTimer1()
{
	unsigned long ulPeriod = 0;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);

	ulPeriod = ( SysCtlClockGet() / FREQ_TIMER1_INTERRUPT);
	TimerLoadSet(TIMER1_BASE, TIMER_A, ulPeriod -1);

	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	IntEnable(INT_TIMER1A);

	//IntMasterEnable();
	TimerEnable(TIMER1_BASE,TIMER_A);

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

void init()
{
	ESP8266_Result_t result;
	//configure the system clock to run at 40MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	initGpios();
	initADC0();
	initTimer0();
	initTimer1();
	initRTC();
	initUART1();
	while( (result = ESP8266_Init(&ESP8266,115200)) != ESP_OK ){
		UARTprintf("Initializing not completed. Result: %d\n", result);
	}

	ESP8266_WaitReady(&ESP8266);

	while (ESP8266_SetMode(&ESP8266, ESP8266_Mode_STA_AP) != ESP_OK);


	/* Disconnect from wifi if connected */
	ESP8266_WifiDisconnect(&ESP8266);

	/* Wait till finish */
	ESP8266_WaitReady(&ESP8266);

	/* Get a list of all stations */
	ESP8266_ListWifiStations(&ESP8266);

	/* Wait till finish */
	ESP8266_WaitReady(&ESP8266);

	/* Connect to wifi and save settings */
	ESP8266_WifiConnect(&ESP8266, "ANDRE_WIFI", "gr68ci49");

	/* Wait till finish */
	ESP8266_WaitReady(&ESP8266);

	/* Get connected devices */
	ESP8266_WifiGetConnected(&ESP8266);

	UARTprintf("Initialization completed\n");


}

