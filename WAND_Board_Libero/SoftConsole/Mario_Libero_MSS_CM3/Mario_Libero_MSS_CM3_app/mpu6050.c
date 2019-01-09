/*
 * mpu6050.c
 *
 *  Created on: Apr 20, 2016
 *      Author: flb
 */

#include "mpu6050.h"

int MPU6050_fifo_error, MPU6050_data_ready;

void mpu_delay()
{
	int i = 0;
	for(i=0; i<1000;i++);
}

int MPU6050_init(void)
{
	// MPU6050 initialization state:
	//
	// Internal 8MHz oscillator
	// Sample rate 1KHz for all sensors
	// Gyroscope bandwidth 42Hz, delay 4.8ms, output freq 1kHz
	// Gyroscope full scale range +-250 deg/sec
	// Accelerometer bandwidth 44KHz, delay 4.9ms
	// Accelerometer full scale range +-2g
	// Motion detection off
	// Interrupts enabled
	// All sensors standby on start-up

	mss_i2c_status_t status;
	uint8_t i2c_read_buf;

	MSS_I2C_init(&g_mss_i2c1, 0, MSS_I2C_PCLK_DIV_120);

	mpu_delay(1000);

	// Set DLPF_CFG to 3
	if (MPU6050_write_reg(MPU6050_CONFIG_REG, 0x03) > 0) return 1;
	if (MPU6050_read_reg(MPU6050_CONFIG_REG, &i2c_read_buf) > 0) return 2;
//	if (i2c_read_buf != 0x03) return 3;

	// Disable temperature sensor
	if (MPU6050_write_reg(MPU6050_PWR_MGMT_1_REG, TEMP_DIS) > 0) return 4;
	if (MPU6050_read_reg(MPU6050_PWR_MGMT_1_REG, &i2c_read_buf) > 0) return 5;
	if (i2c_read_buf != TEMP_DIS) return 6;

	// Gyro on standby and accelerometer running
	if (MPU6050_write_reg(MPU6050_PWR_MGMT_2_REG, STBY_XG | STBY_YG | STBY_ZG) > 0) return 7;
	if (MPU6050_read_reg(MPU6050_PWR_MGMT_2_REG, &i2c_read_buf) > 0) return 8;
	if (i2c_read_buf != (STBY_XG | STBY_YG | STBY_ZG)) return 9;

	// Tell FIFO to accept samples from all accelerometer axes
	if (MPU6050_write_reg(MPU6050_FIFO_EN_REG, ACCEL_FIFO_EN) > 0) return 10;
	if (MPU6050_read_reg(MPU6050_FIFO_EN_REG, &i2c_read_buf) > 0) return 11;
	if (i2c_read_buf != ACCEL_FIFO_EN) return 12;

	//LATCH_INT_EN
	// Configure INT pin to hold until reset
	if (MPU6050_write_reg(MPU6050_INT_PIN_CFG_REG, LATCH_INT_EN) > 0) return 13;
	if (MPU6050_read_reg(MPU6050_INT_PIN_CFG_REG, &i2c_read_buf) > 0) return 14;
	if (i2c_read_buf != LATCH_INT_EN) return 15;

	// Enable the FIFO
	if (MPU6050_write_reg(MPU6050_USER_CTRL_REG, FIFO_EN) > 0) return 16;
	if (MPU6050_read_reg(MPU6050_USER_CTRL_REG, &i2c_read_buf) > 0) return 17;
	if (i2c_read_buf != FIFO_EN) return 18;

	// Clear any interrupt flags
	if (MPU6050_read_reg(MPU6050_INT_STATUS_REG, &i2c_read_buf) > 0) return 19;

	// Enable FIFO overflow and data ready interrupts
	if (MPU6050_write_reg(MPU6050_INT_ENABLE_REG, FIFO_OFLOW_EN | DATA_RDY_EN) > 0) return 20;
	if (MPU6050_read_reg(MPU6050_INT_ENABLE_REG, &i2c_read_buf) > 0) return 21;
	if (i2c_read_buf != (FIFO_OFLOW_EN | DATA_RDY_EN)) return 22;

	// Check I2C status
	status = MSS_I2C_get_status(&g_mss_i2c1);
	if (MSS_I2C_wait_complete(&g_mss_i2c1, MSS_I2C_NO_TIMEOUT) == MSS_I2C_FAILED) return 23;
	if (status == MSS_I2C_FAILED) return 24;

	return 0;
}

uint16_t MPU6050_ISR(uint8_t *buf)
{
	uint8_t i2c_read_1 = 0, i2c_read_2 = 0, int_status = 0;
	uint16_t byte_count = 0;
	int x = 0, y = 0, z = 0;

	// MPU6050 interrupt flags are cleared when the register is read
	if (MPU6050_read_reg(MPU6050_INT_STATUS_REG, &int_status) > 0) return 1;

//	if (int_status & FIFO_OFLOW_INT) {
//		MPU6050_reset_FIFO();
//		return 0;
//	}

	if (int_status &  DATA_RDY_INT) {
		// get high FIFO count byte
		if (MPU6050_read_reg(MPU6050_FIFO_COUNT_H_REG, &i2c_read_1) > 0) return 2;
        byte_count = i2c_read_1 << 8;
x = i2c_read_1;
		// get low FIFO count byte
		if (MPU6050_read_reg(MPU6050_FIFO_COUNT_L_REG, &i2c_read_2) > 0) return 3;
        byte_count += i2c_read_2;
y = i2c_read_2;
z = (x << 8);
z += y;
        if (byte_count < MPU6050_NUM_BYTES_PER_SAMPLE) {
        	return 4;
        }

		MPU6050_read_FIFO(buf, byte_count);

        if (byte_count % MPU6050_NUM_SENSORS*2) {
        	return 5;
        }
	}

	if (int_status & MOT_INT) {
		// motion detect IRQ stub
	}

	if (int_status & I2C_MST_INT) {
		// aux I2C master IRQ stub
	}

	return byte_count;
}

int MPU6050_read_FIFO(uint8_t *buf, uint16_t len)
{
	uint8_t wbuf = MPU6050_FIFO_R_W_REG;

	MSS_I2C_write_read(&g_mss_i2c1, MPU6050_BASE_ADDR, &wbuf, 1, buf, len, MSS_I2C_RELEASE_BUS);
	if (MSS_I2C_wait_complete(&g_mss_i2c1, MSS_I2C_NO_TIMEOUT) == MSS_I2C_FAILED) {
		return 1;
	}
	return 0;
}

int MPU6050_read_reg(uint8_t regnum, uint8_t *regval)
{
	uint8_t wbuf = regnum;

	MSS_I2C_write_read(&g_mss_i2c1, MPU6050_BASE_ADDR, &wbuf, 1, regval, 1, MSS_I2C_RELEASE_BUS);
	if (MSS_I2C_wait_complete(&g_mss_i2c1, MSS_I2C_NO_TIMEOUT) == MSS_I2C_FAILED) {
		return 1;
	}
	return 0;
}

int MPU6050_write_reg(uint8_t regnum, uint8_t regvalue)
{
	static uint8_t buf[2];

	buf[0] = regnum;
	buf[1] = regvalue;

	MSS_I2C_write(&g_mss_i2c1, MPU6050_BASE_ADDR, buf, 2, MSS_I2C_RELEASE_BUS);
	if (MSS_I2C_wait_complete(&g_mss_i2c1, MSS_I2C_NO_TIMEOUT) == MSS_I2C_FAILED) {
		return 1;
	}
	return 0;
}

int MPU6050_reset_FIFO(void)
{
	uint8_t rdval, wrval;

	// read current register settings
	if (MPU6050_read_reg(MPU6050_USER_CTRL_REG, &rdval) > 0) return -1;

	// disable FIFO
	wrval = rdval & ~FIFO_EN;
	if (MPU6050_write_reg(MPU6050_USER_CTRL_REG, wrval) > 0) return -2;

	//reset FIFO (reset bit is cleared when reset completes)
	wrval |= FIFO_RESET;
	if (MPU6050_write_reg(MPU6050_USER_CTRL_REG, wrval) > 0) return -3;

	// restore register contents (re-enables the FIFO)
	if (MPU6050_write_reg(MPU6050_USER_CTRL_REG, rdval) > 0) return -4;

	return 0;
}

