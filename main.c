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
#include "utils/uartstdio.h"
#include "init.h"
#include "fatfs/ff.h"
#include "esp8266/esp8266.h"

 #define RED_LED   GPIO_PIN_1
 #define BLUE_LED  GPIO_PIN_2
 #define GREEN_LED GPIO_PIN_3

#define FILE_NAME_BUFFER_SIZE  32

typedef enum
{
	CREATE_OPEN_NEW_FILE,
	CLOSE_SEND_FILE = SAVING_DATA_COUNT
}SavingState;

typedef enum
{
	OPEN_CONNECTION,
	CLOSE_CONNECTION
}SendingState;

unsigned long soundValuesBuff1[BUFFER_SIZE];
unsigned long soundValuesBuff2[BUFFER_SIZE];

char FileName[FILE_NAME_BUFFER_SIZE];

unsigned long *buff = NULL;
unsigned int transfers = 0;

bool IsReadyToSave = false;
unsigned int filesAvailable = 0;

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
		IsReadyToSave = true;
		transfers = 0;

		// Read the current state of the GPIO pin and
		// write back the opposite state

	}
}

void Timer1AIntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	ESP8266_TimeUpdate(&ESP8266,TIME_TIMER1A_INTERRUPT);

	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) )
	{
		GPIOPinWrite(GPIO_PORTF_BASE, RED_LED| BLUE_LED|GREEN_LED, 0);
	}
	else
	{
		GPIOPinWrite(GPIO_PORTF_BASE, RED_LED| BLUE_LED|GREEN_LED, BLUE_LED);
	}

}

void SaveSensorsData()
{
	static SavingState savingState = CREATE_OPEN_NEW_FILE;
	static unsigned long *buffToSave = NULL;
	static unsigned int fileID = 0;
	UINT bw = 0;

	if( !IsReadyToSave )
		return;

	IsReadyToSave = false;

	switch( savingState )
	{
	case CREATE_OPEN_NEW_FILE:

		usnprintf(FileName,FILE_NAME_BUFFER_SIZE,"S%05d.txt",fileID);
		re = f_open(&Fil, FileName, FA_WRITE | FA_OPEN_ALWAYS );
		if( re == FR_OK )
		{
			UARTprintf("Creating %s...\n", FileName);
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
			UARTprintf("Closing %s...\n", FileName);
			UARTprintf("Preparing to send data...\n");
			filesAvailable++;
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
		break;

	}
}

void SendSensorsData()
{
	static SendingState sendingState = OPEN_CONNECTION;
	ESP8266_Result_t result = ESP_OK;

	if( !filesAvailable )
		return;

	ESP8266_WaitReady(&ESP8266);

	switch( sendingState )
	{
	case OPEN_CONNECTION:
		result = ESP8266_StartClientConnection(&ESP8266, "sensor", "www.google.com", 80,NULL);
		if( result == ESP_OK )
		{
			sendingState = CLOSE_CONNECTION;
		}
		break;
	case CLOSE_CONNECTION:
		result = ESP8266_CloseConnection(&ESP8266,&ESP8266.Connection[0]);
		if( result == ESP_OK )
		{
			filesAvailable--;
			sendingState = OPEN_CONNECTION;
		}
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
   		ESP8266_Update(&ESP8266);

		SaveSensorsData();
		SendSensorsData();

   		if (UARTCharsAvail(UART1_BASE))
   		{
   			UARTCharPut(UART3_BASE, UARTCharGet(UART1_BASE));
   		}

	}
}

/* Called when ready string detected */
void ESP8266_Callback_DeviceReady(ESP8266_t* ESP8266) {
	UARTprintf("Device is ready\r\n");
}

/* Called when watchdog reset on ESP8266 is detected */
void ESP8266_Callback_WatchdogReset(ESP8266_t* ESP8266) {
	UARTprintf("Watchdog reset detected!\r\n");
}

/* Called when we are disconnected from WIFI */
void ESP8266_Callback_WifiDisconnected(ESP8266_t* ESP8266) {
	UARTprintf("Wifi is disconnected!\r\n");
}

void ESP8266_Callback_WifiConnected(ESP8266_t* ESP8266) {
	UARTprintf("Wifi is connected!\r\n");
}

void ESP8266_Callback_WifiConnectFailed(ESP8266_t* ESP8266) {
	UARTprintf("Connection to wifi network has failed. Reason %d\r\n", ESP8266->WifiConnectError);
}

void ESP8266_Callback_WifiGotIP(ESP8266_t* ESP8266) {
	UARTprintf("Wifi got an IP address\r\n");

	/* Read that IP from module */
	UARTprintf("Grabbing IP status: %d\r\n", ESP8266_GetSTAIP(ESP8266));
}

void ESP8266_Callback_WifiIPSet(ESP8266_t* ESP8266) {
	/* We have STA IP set (IP set by router we are connected to) */
	UARTprintf("We have valid IP address: %d.%d.%d.%d\r\n", ESP8266->STAIP[0], ESP8266->STAIP[1], ESP8266->STAIP[2], ESP8266->STAIP[3]);
}

void ESP8266_Callback_DHCPTimeout(ESP8266_t* ESP8266) {
	UARTprintf("DHCP timeout!\r\n");
}

void ESP8266_Callback_WifiDetected(ESP8266_t* ESP8266, ESP8266_APs_t* ESP8266_AP) {
	uint8_t i = 0;

	/* Print number of detected stations */
	UARTprintf("We have detected %d AP stations\r\n", ESP8266_AP->Count);

	/* Print each AP */
	for (i = 0; i < ESP8266_AP->Count; i++) {
		/* Print SSID for each AP */
		UARTprintf("%2d: %s\r\n", i, ESP8266_AP->AP[i].SSID);
	}
}

/************************************/
/*         CLIENT CALLBACKS         */
/************************************/
void ESP8266_Callback_ClientConnectionConnected(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	/* We are connected to external server */
	UARTprintf("Client connected to server! Connection number: %s\r\n", Connection->Name);

	/* We are connected to server, request to sent header data to server */
	ESP8266_RequestSendData(ESP8266, Connection);
}

/* Called when client connection fails to server */
void ESP8266_Callback_ClientConnectionError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	/* Fail with connection to server */
	UARTprintf("An error occured when trying to connect on connection: %d\r\n", Connection->Number);
}

/* Called when data are ready to be sent to server */
uint16_t ESP8266_Callback_ClientConnectionSendData(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size) {
	/* Format data to sent to server */
	usnprintf(Buffer,max_buffer_size, "GET / HTTP/1.1\r\n");
	strcat(Buffer, "Host: stm32f4-discovery.com\r\n");
	strcat(Buffer, "Connection: close\r\n");
	strcat(Buffer, "\r\n");

	/* Return length of buffer */
	return strlen(Buffer);
}

/* Called when data are send successfully */
void ESP8266_Callback_ClientConnectionDataSent(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	UARTprintf("Data successfully sent as client!\r\n");
}

/* Called when error returned trying to sent data */
void ESP8266_Callback_ClientConnectionDataSentError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	UARTprintf("Error while sending data on connection %d!\r\n", Connection->Number);
}

uint32_t time = 0;
void ESP8266_Callback_ClientConnectionDataReceived(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer) {
	/* Data received from server back to client */
	UARTprintf("Data received from server on connection: %s; Number of bytes received: %d; %d / %d;\r\n",
		Connection->Name,
		Connection->BytesReceived,
		Connection->TotalBytesReceived,
		Connection->ContentLength
	);

	/* Print message when first packet */
	if (Connection->Flags.F.FirstPacket) {
		/* Print first message */
		UARTprintf("This is first packet received. Content length on this connection is: %d\r\n", Connection->ContentLength);
	}
}

/* Called when connection is closed */
void ESP8266_Callback_ClientConnectionClosed(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	UARTprintf("Client connection closed, connection: %d; Total bytes received: %d; Content-Length header: %d\r\n",
		Connection->Number, Connection->TotalBytesReceived, Connection->ContentLength
	);

	/* Calculate time */


	/* Print time we need to get data back from server */
	//UARTprintf("Time for data: %u ms; speed: %d kb/s\r\n", time, Connection->TotalBytesReceived / time);
}

/* Called when timeout is reached on connection to server */
void ESP8266_Callback_ClientConnectionTimeout(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	UARTprintf("Timeout reached on connection: %d\r\n", Connection->Number);
}

