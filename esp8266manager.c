/*
 * wifi.c
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "esp8266/esp8266.h"
#include "filemanager.h"
#include "utils/uartstdio.h"

#define TIME_TIMER1A_INTERRUPT 100 //ms
#define TIMER1_INTERRUPT_FREQUENCY (1000/TIME_TIMER1A_INTERRUPT)

#define TIMEOUT_RECONNECTION 300

typedef enum
{
	OPEN_CONNECTION,
	WAITING_CONNECTION,
	REQUEST_SEND_DATA,
	SENDING_DATA,
	WAIT_RECONNECT,
	CLOSE_CONNECTION,
	WAITING_CLOSE
}ConnState;

typedef struct{
	ESP8266_t dev;
	struct {
		ConnState conn;
	}state;
	struct {
		unsigned int _100MS;
	}flg;

}ESP8266_MANAGER_DEF;

static ESP8266_MANAGER_DEF ESP8266;

void Timer1AIntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	ESP8266_TimeUpdate(&ESP8266.dev,TIME_TIMER1A_INTERRUPT);
	ESP8266.flg._100MS = 1;
}

void initTimer1()
{
	unsigned long ulPeriod = 0;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);

	ulPeriod = ( SysCtlClockGet() / TIMER1_INTERRUPT_FREQUENCY);
	TimerLoadSet(TIMER1_BASE, TIMER_A, ulPeriod -1);

	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	IntEnable(INT_TIMER1A);

	TimerEnable(TIMER1_BASE,TIMER_A);

}

void initESP8266()
{
	initTimer1();

	ESP8266.state.conn = OPEN_CONNECTION;

	ESP8266_Init(&ESP8266.dev,115200);

	/* Wait till finish */
	ESP8266_WaitReady(&ESP8266.dev);

	ESP8266_SetAutoConnect(&ESP8266.dev,ESP8266_AutoConnect_Off);

	/* Wait till finish */
	ESP8266_WaitReady(&ESP8266.dev);

	ESP8266_SetMode(&ESP8266.dev, ESP8266_Mode_STA_AP);

	/* Wait till finish */
	ESP8266_WaitReady(&ESP8266.dev);

	/* Get a list of all stations */
	ESP8266_ListWifiStations(&ESP8266.dev);

	/* Wait till finish */
	ESP8266_WaitReady(&ESP8266.dev);

	/* Connect to wifi and save settings */
	ESP8266_WifiConnect(&ESP8266.dev, "ANDRE_WIFI", "gr68ci49");

	/* Wait till finish */
	ESP8266_WaitReady(&ESP8266.dev);

	/* Get connected devices */
	ESP8266_WifiGetConnected(&ESP8266.dev);

	/* Wait till finish */
	ESP8266_WaitReady(&ESP8266.dev);

	UARTprintf("Initialization completed\n");

}

/*
 *
 */
void ESP8266Handler()
{
	static unsigned int timeToReconnect = 0;

	ESP8266_Update(&ESP8266.dev);

	if( !IsThereFileToSend() )return;
	if( ESP8266_IsReady(&ESP8266.dev) == ESP_BUSY )return;

	switch( ESP8266.state.conn )
	{
		case OPEN_CONNECTION:
			ESP8266_StartClientConnection(&ESP8266.dev, "fdamost", "192.168.0.105", 5000, &ESP8266.state.conn);
			ESP8266.state.conn = WAITING_CONNECTION;
			break;
		case WAITING_CONNECTION:
			break;
		case SENDING_DATA:
			break;
		case WAITING_CLOSE:
			break;
		case WAIT_RECONNECT:
			if( ESP8266.flg._100MS )
			{
				ESP8266.flg._100MS = 0;
				timeToReconnect++;
				if( timeToReconnect == TIMEOUT_RECONNECTION )
				{
					timeToReconnect = 0;
					ESP8266.state.conn = OPEN_CONNECTION;
				}
			}
			break;

	}

	if (UARTCharsAvail(UART1_BASE))
	{
		UARTCharPut(UART3_BASE, UARTCharGet(UART1_BASE));
	}

}

/************************************/
/*		 CALLBACKS         			*/
/************************************/
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
}//

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
	*((ConnState *)(Connection->UserParameters)) = SENDING_DATA;
	ESP8266_RequestSendData(ESP8266, Connection);

}

/* Called when client connection fails to server */
void ESP8266_Callback_ClientConnectionError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	/* Fail with connection to server */
	UARTprintf("An error occured when trying to connect on connection: %d\r\n", Connection->Number);
	*((ConnState *)(Connection->UserParameters)) =  WAIT_RECONNECT;
}

/* Called when data are ready to be sent to server */
uint16_t ESP8266_Callback_ClientConnectionSendData(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size) {
	/* Format data to sent to server */
	return FileReadHandler(Buffer, max_buffer_size);
}

/* Called when data are send successfully */
void ESP8266_Callback_ClientConnectionDataSent(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	UARTprintf("Data successfully sent as client!\r\n");
	if( IsThereFileToSend() || !ReadingFinished() )
	{
		*((ConnState *)(Connection->UserParameters)) = SENDING_DATA;
		ESP8266_RequestSendData(ESP8266, Connection);
	}
	else
	{
		*((ConnState *)(Connection->UserParameters)) =  WAITING_CLOSE;
		ESP8266_CloseConnection(ESP8266, Connection);
	}

}

/* Called when error returned trying to sent data */
void ESP8266_Callback_ClientConnectionDataSentError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	UARTprintf("Error while sending data on connection %d!\r\n", Connection->Number);

	*((ConnState *)(Connection->UserParameters)) =  WAITING_CLOSE;
	ESP8266_CloseConnection(ESP8266, Connection);
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

	if ( *((ConnState *)(Connection->UserParameters)) == WAITING_CONNECTION )
	{
		*((ConnState *)(Connection->UserParameters)) = WAIT_RECONNECT;
	}
	else
	{
		*((ConnState *)(Connection->UserParameters)) = OPEN_CONNECTION;
	}
}

/* Called when timeout is reached on connection to server */
void ESP8266_Callback_ClientConnectionTimeout(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	UARTprintf("Timeout reached on connection: %d\r\n", Connection->Number);
}

