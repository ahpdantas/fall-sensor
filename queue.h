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
Enqueue(QUEUE q, unsigned int Element);
Dequeue(QUEUE q, unsigned int* Element);
Destroy(QUEUE q);

#endif /* QUEUE_H_ */
