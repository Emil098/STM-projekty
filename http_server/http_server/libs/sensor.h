/*
 * dht22_handler.h
 *
 *  Created on: Feb 20, 2026
 *      Author: emilr
 */

#ifndef SENSOR_H_
#define SENSOR_H_

void init_sensor();
void update_sensor();
void get_sensor(float *temp, float *hum);

#endif /* SENSOR_H_ */
