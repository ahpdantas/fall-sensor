/*
 * amost.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

int initVolume();

int IsThereFileToSend();
int ReadingFinished();
void FileWriteHandler(char* Buffer, unsigned int bufferSize );
int FileReadHandler(char* Buffer, unsigned int bufferSize );

#endif /* FILEMANAGER_H_ */
