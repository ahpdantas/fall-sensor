/**	
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
 */
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "esp8266_ll.h"

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*                                                                        */
/*    Edit file name to esp8266_ll.c and edit values for your platform    */
/*                                                                        */
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

uint8_t ESP8266_LL_USARTInit(uint32_t baudrate) {
	/* Init USART */
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
	SysCtlPeripheralEnable(SYSCTL_PERIPH2_GPIOC);

	GPIOPinConfigure(GPIO_PC6_U3RX);
	GPIOPinConfigure(GPIO_PC7_U3TX);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7 );

	UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), baudrate, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE );


#if ESP8266_USE_CTS
    /* Enable RTS pin on MCU as output and set it low */
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_5);
	GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,0);
#endif

	//UARTFIFOLevelSet(UART3_BASE, UART_FIFO_TX4_8, UART_FIFO_RX4_8); //set FIFO level to 8 characters
	//UARTFIFOEnable(UART3_BASE); //enable FIFOs

	UARTIntEnable(UART3_BASE, UART_INT_RX | UART_INT_RT); //enable Receiver interrupts
	IntEnable(INT_UART3); //enable the UART interrupt

	/* Return 0 = Successful */
	return 0;
}

uint8_t ESP8266_LL_USARTSend(uint8_t* data, uint16_t count) {
	/* Send data via USART */
	uint16_t dataSent = 0;
	while( dataSent != count )
	{
		UARTCharPut(UART3_BASE,data[dataSent]);
		UARTCharPut(UART1_BASE,data[dataSent]);
		dataSent++;
	}
	/* Return 0 = Successful */
	return 0;
}


void ESP8266_LL_SetRTS(uint8_t dir) {
    /* Set RTS direction we require */
    if (dir == ESP_RTS_HIGH) {
    	GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,GPIO_PIN_5);
    } else {
    	GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,0);
    }
}


/* USART receive interrupt handler */
void USART_RX_INTERRUPT_HANDLER_FUNCTION_NAME(void) {
	uint8_t ch;
	unsigned long ulStatus;

	/* Get character from USART */
	ulStatus = UARTIntStatus(UART3_BASE, true); //get interrupt status
	UARTIntClear(UART3_BASE, ulStatus); //clear the asserted interrupts
	while (UARTCharsAvail(UART3_BASE)) //loop while there are characters in the receive FIFO
	{
		ch = UARTCharGet(UART3_BASE);
		UARTCharPut(UART1_BASE,ch);
		/* Send received character to ESP stack */
		ESP8266_DataReceived(&ch, 1);
	}
}
