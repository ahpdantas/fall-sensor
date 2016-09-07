/*
 * wifi.h
 *
 *  Created on: 6 de set de 2016
 *      Author: andre
 */

#ifndef WIFI_H_
#define WIFI_H_

#include "fallsensor.h"
#include "esp8266/esp8266.h"

void initESP8266(ESP8266_t* ESP8266);
void ESP8266Handler( FALL_SENSOR_DEF* Fall );

#endif /* WIFI_H_ */
