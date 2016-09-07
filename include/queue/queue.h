/*
 * queue.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct QUEUE_DEF* QUEUE;

QUEUE CreateQueue();
void Enqueue(QUEUE q, unsigned int value);
int IsEmpty(QUEUE q);
int Dequeue(QUEUE q, unsigned int* value);
void Destroy(QUEUE q);

#endif /* QUEUE_H_ */
