/*
 * wifi.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef ESP8266MANAGER_H_
#define ESP8266MANAGER_H_

typedef struct ESP8266_MANAGER_DEF *ESP8266_MANAGER;

void initESP8266(ESP8266_MANAGER* ESP8266);
void ESP8266Handler( ESP8266_MANAGER ESP8266 );

#endif /* ESP8266MANAGER_H_ */
