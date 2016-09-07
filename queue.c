/*
 * queue.c
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */
#include "queue.h"
#include "utils/uartstdio.h"
typedef struct
{

}QUEUE_DEF;


QUEUE CreateQueue()
{
	UARTprintf("File Queue created.\n");
	return 0;
}

Enqueue(QUEUE q, unsigned int Element)
{
	UARTprintf("Enqueue.\n");
}

Dequeue(QUEUE q, unsigned int* Element)
{
	UARTprintf("Dequeue.\n");
}

Destroy(QUEUE q)
{

}
