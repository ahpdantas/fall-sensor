/*
 * queue.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef QUEUE_H_
#define QUEUE_H_

typedef QUEUE_DEF* QUEUE;

Queue Create();
Enqueue(QUEUE q, void* Element);
Dequeue(QUEUE q, void* Element);
Destroy(QUEUE q);

#endif /* QUEUE_H_ */
