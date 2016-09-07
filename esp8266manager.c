/*
 * wifi.c
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */
#include "fallsensor.h"
#include "esp8266/esp8266.h"
#include "utils/uartstdio.h"

typedef enum
{
	OPEN_CONNECTION,
	SEND_DATA,
	CLOSE_CONNECTION
}ConnState;

typedef struct{
	ESP8266_t dev;
	struct {
		ConnState conn;
	}state;
}ESP8266_MANAGER_DEF;
/*
void SendSensorsData()
{
	ESP8266_Result_t result = ESP_OK;
	static ConnectionData conn = { .state = OPEN_CONNECTION };

	if( !filesAvailable )
		return;

	ESP8266_WaitReady(&ESP8266);

	switch( conn.state )
	{
	case OPEN_CONNECTION:
		ESP8266_StartClientConnection(&ESP8266, "sensor", "192.168.0.105", 5000, &conn);
		if( (result = ESP8266_IsReady(&ESP8266)) == ESP_OK  )
		{
			conn.state = SEND_DATA;
		}
		break;
	//implemented using user callbacks
	case SEND_DATA:
		result = ESP8266_RequestSendData(&ESP8266, &ESP8266.Connection[0]);
		break;
	case CLOSE_CONNECTION:
		result = ESP8266_CloseConnection(&ESP8266,&ESP8266.Connection[0]);
		if( result == ESP_OK )
		{
			conn.state = OPEN_CONNECTION;
		}
		break;

	}
}
*/
void initESP8266(ESP8266_MANAGER* ESP8266)
{
	ESP8266_MANAGER_DEF *ESP = NULL;

	ESP = malloc(sizeof(ESP8266_MANAGER_DEF));

	ESP->state.conn = OPEN_CONNECTION;

	ESP8266_Init(&ESP->dev,115200);

	/* Wait till finish */
	ESP8266_WaitReady(&ESP->dev);

	ESP8266_SetAutoConnect(&ESP->dev,ESP8266_AutoConnect_Off);

	/* Wait till finish */
	ESP8266_WaitReady(&ESP->dev);

	ESP8266_SetMode(&ESP->dev, ESP8266_Mode_STA_AP);

	/* Wait till finish */
	ESP8266_WaitReady(&ESP->dev);

	/* Get a list of all stations */
	ESP8266_ListWifiStations(&ESP->dev);

	/* Wait till finish */
	ESP8266_WaitReady(&ESP->dev);

	/* Connect to wifi and save settings */
	ESP8266_WifiConnect(&ESP->dev, "ANDRE_WIFI", "gr68ci49");

	/* Wait till finish */
	ESP8266_WaitReady(&ESP->dev);

	/* Get connected devices */
	ESP8266_WifiGetConnected(&ESP->dev);

	/* Wait till finish */
	ESP8266_WaitReady(&ESP->dev);

	*ESP8266 = (ESP_MANAGER)ESP;

	UARTprintf("Initialization completed\n");

}

/*
 *
 */
void ESP8266Handler( ESP8266_MANAGER ESP8266 )
{
	//ESP8266_Update(&Fall->ESP8266);
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
	//ESP8266_RequestSendData(ESP8266, Connection);
}

/* Called when client connection fails to server */
void ESP8266_Callback_ClientConnectionError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	/* Fail with connection to server */
	UARTprintf("An error occured when trying to connect on connection: %d\r\n", Connection->Number);
}

//#define MAX_BYTE_READ 512
/* Called when data are ready to be sent to server */
uint16_t ESP8266_Callback_ClientConnectionSendData(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size) {
	//FALL_SENSOR_DEF *f = (FALL_SENSOR_DEF *)Connection[0]->UserParameters;


	/*
	static FileState fileState = OPEN_FILE;
	static FIL Fread;
	static FRESULT re;
	UINT br = 0;

	// Format data to sent to server
	switch(fileState)
	{
	case OPEN_FILE:
		usnprintf(FileName,FILE_NAME_BUFFER_SIZE,"S%05d.txt",fileToSave);
		re = f_open(&Fread, FileName, FA_READ);
		if( re == FR_OK )
		{
			UARTprintf("Openning the file: %s to send information", FileName );
			fileState = READ_FILE;
		}
		else
		{
			UARTprintf("It was not possible to open the file: %s - ERROR: %d", FileName, re );

		}
	case READ_FILE:
		re = f_read(&Fread,Buffer,MAX_BYTE_READ,&br);
		if( re == FR_OK && !br )
		{
			fileState = CLOSE_FILE;
		}
		break;
	case CLOSE_FILE:
		re = f_close(&Fread);
		if( re == FR_OK )
		{
			UARTprintf("File %s closed with success.", FileName);
			fileState = OPEN_FILE;
			filesAvailable--;
			((ConnectionData *)Connection->UserParameters)->state = CLOSE_CONNECTION;
		}
		else
		{
			UARTprintf("It was not possible to close the file: %s - ERROR: %d", FileName, re);
		}
		break;
	}

	return br;*/
}

/* Called when data are send successfully */
void ESP8266_Callback_ClientConnectionDataSent(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	UARTprintf("Data successfully sent as client!\r\n");
	//((ConnectionData *)Connection->UserParameters)->state = CLOSE_CONNECTION;
	//fileToSave++;
}

/* Called when error returned trying to sent data */
void ESP8266_Callback_ClientConnectionDataSentError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	UARTprintf("Error while sending data on connection %d!\r\n", Connection->Number);
	//((ConnectionData *)Connection->UserParameters)->state = CLOSE_CONNECTION;
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

	//((ConnectionData *)Connection->UserParameters)->state = CLOSE_CONNECTION;
}

/* Called when timeout is reached on connection to server */
void ESP8266_Callback_ClientConnectionTimeout(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection) {
	UARTprintf("Timeout reached on connection: %d\r\n", Connection->Number);
	//((ConnectionData *)Connection->UserParameters)->state = CLOSE_CONNECTION;
}

