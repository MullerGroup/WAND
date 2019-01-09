/*
 * accelerometer.h
 *
 *  Created on: May 20, 2016
 *      Author: subnets
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#define SF2_CLK_MHZ 20
#define MPU6050_FIFO_DELAY (SF2_CLK_MHZ * 100000) // ~100ms with a 20MHz source
//#define MPU6050_FIFO_DELAY 2000000 // ~100ms with a 20MHz source

void accel_init(void);
void accel_FIFOread(void);
uint8_t * accel_sample_read(void);

#endif /* ACCELEROMETER_H_ */



