/*
 * queue.c
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */
#include <stdlib.h>
#include "queue.h"
#include "utils/uartstdio.h"

typedef struct NODE_DEF* NODE;

typedef struct
{
	unsigned int value;
	NODE next;
	NODE prev;
}NODE_DEF;

typedef struct
{
	unsigned int size;
	NODE back;
	NODE front;
}QUEUE_DEF;


QUEUE CreateQueue()
{
	QUEUE_DEF *q = malloc(sizeof(QUEUE_DEF));
	q->back = NULL;
	q->front = NULL;
	q->size = 0;
	return q;
}

void Enqueue(QUEUE q, unsigned int value )
{
	//create node structure
	NODE_DEF* n = malloc(sizeof(NODE_DEF));

	//initialize node structure
	n->value = value;
	n->next = NULL;
	n->prev = NULL;

	//adding the node
	if( ((QUEUE_DEF*)q)->size == 0 )
	{
		((QUEUE_DEF*)q)->back = (NODE)n;
		((QUEUE_DEF*)q)->front = (NODE)n;
	}
	else
	{
		((NODE_DEF*)(((QUEUE_DEF*)q)->back))->prev = (NODE)n;
		n->next =((QUEUE_DEF*)q)->back;
		((QUEUE_DEF*)q)->back = (NODE)n;
	}
	((QUEUE_DEF*)q)->size++;
}

int IsEmpty(QUEUE q)
{
	return ((QUEUE_DEF*)q)->size;
}

int Dequeue(QUEUE q, unsigned int *value)
{
	NODE_DEF* front;
	NODE_DEF* prev;

	front = (NODE_DEF*)((QUEUE_DEF*)q)->front;
	prev = (NODE_DEF*)front->prev;

	if( ((QUEUE_DEF*)q)->size > 0 )
	{
		if( value != NULL )
		{
			*value = front->value;
		}
		prev->next = NULL;
		((QUEUE_DEF*)q)->front = (NODE)prev;
		free(front);
		((QUEUE_DEF*)q)->size--;
	}
	else
	{
		return -1;
	}

	return 0;
}

void Destroy(QUEUE q)
{
	while( ((QUEUE_DEF*)q)->size > 0 )
	{
		Dequeue(q,NULL);
	}
	free(q);
}
