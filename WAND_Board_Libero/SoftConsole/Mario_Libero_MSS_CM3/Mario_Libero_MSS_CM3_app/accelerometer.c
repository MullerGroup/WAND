/*
 * accelerometer.c
 *
 *  Created on: May 20, 2016
 *      Author: subnets
 */

#include "mpu6050.h"
#include "accelerometer.h"
#include "mss_timer.h"
#include "mss_gpio.h"
#include "mss_assert.h"

uint8_t mpu6050_fifo_buf[1024];
uint8_t *buffer_read_pointer = &mpu6050_fifo_buf[0];
uint16_t mpu6050_fifo_bytecount;
int buffer_read_counter = 0;
uint8_t retval;

void accel_init(void)
{
	// Initialize GPIOs (port 0: MPU6050 interrupt, port 1: ACC_CLKIN, ports 2-5: gpio2-5)
	MSS_GPIO_init();
	MSS_GPIO_config(MSS_GPIO_5, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE);
	MSS_GPIO_config(MSS_GPIO_4, MSS_GPIO_OUTPUT_MODE);
	MSS_GPIO_set_output(MSS_GPIO_4, 0);
	MSS_GPIO_enable_irq(MSS_GPIO_5);

	MSS_TIM1_init(MSS_TIMER_ONE_SHOT_MODE);
	MSS_TIM1_load_immediate(MPU6050_FIFO_DELAY);
	MSS_TIM1_enable_irq();

	// Any non-zero return from MPU6050_init() means that an I2C API function failed. This is a fatal error (at present).
	// MPU6050_init() will block if an I2C function does not return. This is also a fatal error and indicates a bug in the code.
	retval = MPU6050_init();
	ASSERT(retval == 0);
}

    // Before NM main():
    //     accel_init();
    // Belongs in NM main():
	//     if (mpu6050_irq_active) {
	//         mpu6050_irq_active = 0;
    //         accel_FIFOread();


void accel_FIFOread(void)
{
	//The following two lines pull data from the FIFO buffer on the MPU and move it into mpu6050_fifo_buf
    //Then, since new data is in the buffer, the read pointer is set back to the beginning of the buffer where it will be manipulated by accel_sample_read()
    mpu6050_fifo_bytecount = MPU6050_ISR(mpu6050_fifo_buf);
	buffer_read_pointer = &mpu6050_fifo_buf[0];
    buffer_read_counter = 0;
    //The following was commented out because it instead of copying data from the mpu6050_fifo_buf to another buffer, data will be retrieved straight from the mpu_fifo_buf
    //Put write pointer at beginning of buffer that data from the FIFO will be placed in
    //buffer_write_pointer = buf;
	//int i = 0;
	//if (mpu6050_fifo_bytecount > 0) {
		//uint16_t accelXval, accelYval, accelZval;
        //Take data from FIFO and put into buffer
		//while (i < mpu6050_fifo_bytecount && i < 1024) {
			//accelXval = (mpu6050_fifo_buf[i] << sizeof(uint8_t)) + mpu6050_fifo_buf[i+1];
			//accelYval = (mpu6050_fifo_buf[i+2] << sizeof(uint8_t)) + mpu6050_fifo_buf[i+3];
			//accelZval = (mpu6050_fifo_buf[i+4] << sizeof(uint8_t)) + mpu6050_fifo_buf[i+5];
			//*buffer_write_pointer = accelXval;
            //buffer_write_pointer += 2;
            //*buffer_write_pointer = accelYval;
            //buffer_write_pointer += 2;
            //*buffer_write_pointer = accelZval:
            //buffer_write_point += 2;
            //i += 6;
}
//}
//}

//The following code will loop through the MPU FIFO buffer and return addresses to where data should be retrieved from
//The majority was commented out because assumptions were taken into consideration after it was originally written and much of it was no longer necessary
uint8_t * accel_sample_read(void)
{
    //static int previous_read_size;
    //The following tests to make sure that the loop has already run.
    //The read pointer (buffer_read_pointer) and the value of the remaining left to be read (buffer_read_remainder) won't change until the second time the function is called
    if (buffer_read_counter >= MPU6050_MAX_SAMPLES_IN_FIFO)
    {
    	return 0;
    }
	if (buffer_read_counter >= 1)
    {
        buffer_read_pointer += MPU6050_NUM_BYTES_PER_SAMPLE;
        //buffer_read_remainder -= previous_read_size;
    }
    //The following makes sure that there is enough data left in the FIFO by making sure the remaining left to be read (buffer_read_remainder) isn't smaller than the read size
    //If the expression is true, the previous_read_size is changed to ensure that the pointer and remainder don't change next time the loop is run because data wasn't retrieved yet
    //Then, an error is returned with the value of the remainder which can then be used as the read_size next time the function is called
    //if buffer_read_remainder < read_size && buffer_read_remainder > 0
    //{
        //previous_read_size = 0
        //return -buffer_read_remainder;
    //}
    //If the remainder is zero, the pointer must be returned to the beginning of the mpu FIFO buffer and the remainder must be reset to 0
    //else if buffer_read_remainder == 0
    //{
        //buffer_read_pointer = mpu6050_fifo_buf[0];
        //buffer_read_remainder = 1024;
    //}
    //previous_read_size keeps track of how much the buffer_read_pointer and remainder should change next time the function is called because a different read_size may be used then
    //previous_read_size = read_size;
    buffer_read_counter++;
    //The address at which data should be retrieved from is returned
    return buffer_read_pointer;
}


