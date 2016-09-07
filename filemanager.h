/*
 * amost.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

typedef struct FILE_MANAGER_DEF * FILE_MANAGER;

int initVolume(FILE_MANAGER *FileManager);
void FileWriteHandler(FILE_MANAGER FileManager, char* Buffer, unsigned int bufferSize );
int FileReadHandler(FILE_MANAGER FileManager, char* Buffer, unsigned int bufferSize);

#endif /* FILEMANAGER_H_ */
