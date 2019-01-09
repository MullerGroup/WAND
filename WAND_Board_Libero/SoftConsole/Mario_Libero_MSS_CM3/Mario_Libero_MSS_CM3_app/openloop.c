// begin working on channel selection and accelerometer integration

#include <m2sxxx.h>
#include "mss_uart.h"
#include <stdio.h>
#include "mss_spi.h"
#include <mss_hpdma.h>
#include "mss_gpio.h"
#include "mss_timer.h"
#include <stdbool.h>
#include "accelerometer.h"


// ring buffer for command bytes received from GUI
#define RINGBUF_SIZE 64
static volatile char ringbuf[RINGBUF_SIZE];
static volatile unsigned int rbwrptr = 0;
static unsigned int rbrdptr = 0;
#define BYTES_IN_RINGBUF() ((rbwrptr - rbrdptr) & (RINGBUF_SIZE-1))

// NM registers
#define NM_CHIP_ID_REG 0x00
#define NM_STATUS_REG 0x01
#define NM_PWR_CONFIG_REG 0x02
#define NM_TEST_SEL_REG 0x03
#define NM_REC_ENABLE0_REG 0x04
#define NM_REC_ENABLE1_REG 0x05
#define NM_REC_ENABLE2_REG 0x06
#define NM_REC_ENABLE3_REG 0x07
#define NM_IMP_EN0_REG 0x08
#define NM_IMP_EN1_REG 0x09
#define NM_IMP_EN2_REG 0x0A
#define NM_IMP_EN3_REG 0x0B
#define NM_REC_CONFIG_REG 0x0C
#define NM_SYS_CONFIG_REG 0x0D
#define RESERVED1 0x0E
#define NM_SCRATCH_REG 0x0F

#define NM_STIM_CFG0_REG 0x10
#define NM_STIM_CFG1_REG 0x11
#define NM_STIM_CFG2_REG 0x12
#define NM_STIM_CFG3_REG 0x13
#define NM_STIM_CFG4_REG 0x14
#define NM_STIM_CFG5_REG 0x15
#define NM_STIM_CFG6_REG 0x16
#define NM_STIM_CFG7_REG 0x17
#define NM_STIM_CFG8_REG 0x18
#define NM_STIM_CFG9_REG 0x19
#define NM_STIM_CFG10_REG 0x1A
#define NM_STIM_CFG11_REG 0x1B
#define NM_STIM_CFG12_REG 0x1C
#define NM_STIM_CFG13_REG 0x1D
#define NM_STIM_CFG14_REG 0x1E
#define NM_STIM_CFG15_REG 0x1F

// SF2 "registers"
#define REG_BASE 0x30000000
int *status = (int*)(REG_BASE+0);
int *rst = (int*)(REG_BASE+4);
int *dbg = (int*)(REG_BASE+8);
int *n0d1 = (int*)(REG_BASE+0x10);
int *n0d2 = (int*)(REG_BASE+0x14);
int *n0ack = (int*)(REG_BASE+0x18);
int *n0adc = (int*)(REG_BASE+0x1c);
int *n1d1 = (int*)(REG_BASE+0x20);
int *n1d2 = (int*)(REG_BASE+0x24);
int *n1ack = (int*)(REG_BASE+0x28);
int *n1adc = (int*)(REG_BASE+0x2c);

// function for reading in bytes received over SPI from Nordic
void spi_rx_handler();

// buffers for selected data to be transmitted back to the GUI
#define NUM_ENABLED_CHANNELS 96
#define DATA_BUF_SIZE 100
#define NUM_DATA_BUF 8

static volatile uint16_t adc_data_buf[NUM_DATA_BUF][DATA_BUF_SIZE];
static volatile uint16_t dummy;
// Also need a separate buffer for register data, since buf1 and buf2 can't be overwritten.
static uint16_t reg_data_buf[DATA_BUF_SIZE];
// vector for indicating which channels are enabled for transmitting
static uint8_t sel_data_vector[128];

//static uint8_t * bp;	//pointer for accel data
uint8_t mpu6050_irq_active = 0;

static bool stream_en = 0; // for streaming mode
static bool stream_on = 0;
static bool stream_off = 0;

static bool artifact_en = 0; // whether we should do artifact cancellation or not
static bool artifact_on = 0;
static bool artifact_off = 0;

static uint16_t counter = 0;

static bool stim_flag = 0; // true if there was stim on current adc sample
static int chan = 1;
static uint8_t buf_save = 0;
static uint8_t buf_good = NUM_DATA_BUF - 1;
static uint8_t buf_tx = 1;
static uint8_t stim_count = 0;
static uint16_t step = 0;

static uint8_t spi_rx_buf[6];
static volatile unsigned int gui_request_counter = 0;

static int reg_val;
static int reg_addr;
static int reg_data;

static int stim_nm0 = 0; // stim on NM0
static int stim_nm1 = 0; // stim on NM1

static uint32_t stim_rep = 0;

static bool rec_enable_write = false;

#define RST_START   (1 << 8)
#define NM0_START   (1 << 12)
#define NM1_START   (1 << 13)
#define RST_BUSY    (1 << 16)
#define NM0_DATA    (1 << 20)
#define NM1_DATA    (1 << 21)
#define NM0_TXBUSY  (1 << 24)
#define NM1_TXBUSY  (1 << 25)
#define NM0_ACK (1 << 28)
#define NM1_ACK (1 << 29)
#define RST_MODE_AM 0x3
#define RST_MODE_NM 0x2
#define OP_READ 0
#define OP_WRITE 1

int stat = 0;
bool stat_flag = false;

void writeNMreg(int nmid, int nm_addr, int data)
{
	int i;

	switch (nmid){
	case 0:
		// need to update the channel select vector if writing to NM_REC_ENABLE reg
		if (nm_addr == NM_REC_ENABLE0_REG)
		{
			// channels 0-15
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i] = 1;
				}
				else
				{
					sel_data_vector[i] = 0;
				}
			}
		}
		else if (nm_addr == NM_REC_ENABLE1_REG)
		{
			// channels 16-31
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+16] = 1;
				}
				else
				{
					sel_data_vector[i+16] = 0;
				}
			}
		}
		else if (nm_addr == NM_REC_ENABLE2_REG)
		{
			// channels 32-47
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+32] = 1;
				}
				else
				{
					sel_data_vector[i+32] = 0;
				}
			}
		}
		else if (nm_addr == NM_REC_ENABLE3_REG)
		{
			// channels 48-63
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+48] = 1;
				}
				else
				{
					sel_data_vector[i+48] = 0;
				}
			}
		}

		*n0d2 = (nm_addr << 16) | data;
		*n0d1 = OP_WRITE;						// select register operation (1 = write, 0 = read
		*status = NM0_START;					// set
		while((*status) & NM0_TXBUSY);
		break;
	case 1:
		// need to update the channel select vector if writing to NM_REC_ENABLE reg
		if (nm_addr == NM_REC_ENABLE0_REG)
		{
			// channels 64-79
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+64] = 1;
				}
				else
				{
					sel_data_vector[i+64] = 0;
				}
			}
		}
		else if (nm_addr == NM_REC_ENABLE1_REG)
		{
			// channels 80-95
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+80] = 1;
				}
				else
				{
					sel_data_vector[i+80] = 0;
				}
			}
		}
		else if (nm_addr == NM_REC_ENABLE2_REG)
		{
			// channels 96-111
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+96] = 1;
				}
				else
				{
					sel_data_vector[i+96] = 0;
				}
			}
		}
		else if (nm_addr == NM_REC_ENABLE3_REG)
		{
			// channels 112-127
			for (i=0; i<16; i++)
			{
				if (((data >> i) & 1) == 1)
				{
					sel_data_vector[i+112] = 1;
				}
				else
				{
					sel_data_vector[i+112] = 0;
				}
			}
		}

		*n1d2 = (nm_addr << 16) | data;
		*n1d1 = OP_WRITE;						// select register operation (1 = write, 0 = read
		*status = NM1_START;					// set
		while((*status) & NM1_TXBUSY);
		break;
	default:
		break;
	}
}

void NMstim(int command, int nmid)
{
    switch (nmid)
    {
        case 0:
            *n0d2 = command;
            *status = 0x1010;					// set
            while((*status) & NM0_TXBUSY);
            break;
        case 1:
            *n1d2 = command;
            *status = 0x2020;					// set
            while((*status) & NM1_TXBUSY);
            break;
        default:
            break;
    }
}

int readNMreg(int nmid, int addr)
{
	int ret, tmp, crcok, i;
	switch (nmid){
		case 0:
			*n0d2 = addr << 16;
			*n0d1 = OP_READ;
			*status = NM0_START;
			while(!(*status & NM0_ACK));
			for(i = 0; i < 4; i++)
			{
				tmp = *n0ack;
				if (i == 1)
					crcok = (tmp == addr);
				if (i == 2)
					ret = tmp;
				if (i == 3)
					crcok &= (tmp & 1);
			}
			return crcok ? ret : -1;
			break;
		case 1:
			*n1d2 = addr << 16;
			*n1d1 = OP_READ;
			*status = NM1_START;
			while(!(*status & NM1_ACK));
			for(i = 0; i < 4; i++)
			{
				tmp = *n1ack;
				if (i == 1)
					crcok = (tmp == addr);
				if (i == 2)
					ret = tmp;
				if (i == 3)
					crcok &= (tmp & 1);
			}
			return crcok ? ret : -1;
			break;
		default:
			return 0;
			break;
		}
}

void NMreset(int nmid)
{
	switch (nmid){
	case 0:
		*n0d2 = (1 << 10) | 1;
		*n0d1 = 0;
		*status = NM0_START | (1 << 4);					// set to command send mode
		while((*status) & NM0_TXBUSY);
		break;
	case 1:
		*n1d2 = (1 << 10) | 1;
		*n1d1 = 0;
		*status = NM1_START | (1 << 5);					// set to command send mode
		while((*status) & NM1_TXBUSY);
		break;
	default:
		break;
	}
}


int main() {

	SYSREG->SOFT_RST_CR |= SYSREG_FPGA_SOFTRESET_MASK;
	SYSREG->SOFT_RST_CR &= ~SYSREG_FPGA_SOFTRESET_MASK;

	// Initialize SPI_0
	MSS_SPI_init(&g_mss_spi0);
	MSS_SPI_configure_master_mode(&g_mss_spi0,
			MSS_SPI_SLAVE_0,
			MSS_SPI_MODE0,
//			12u, // changed to 12 so that we get 2.5MHz from the new 30MHz apb clock
			8u, // 20MHz/8 = 2.5MHz clock (max nordic is 8MHz)
//			6u,
//            26u,
			MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE); //needed for transmitting in blocks

//	accel_init();

	int i = 0, j = 0;
	uint8_t buf_idx;

	writeNMreg(0, NM_REC_CONFIG_REG, 0x3884); // turn on rec_en
	writeNMreg(1, NM_REC_CONFIG_REG, 0x3884); // turn on rec_en
	writeNMreg(0, NM_REC_CONFIG_REG, 0x3884); // turn on rec_en
	writeNMreg(1, NM_REC_CONFIG_REG, 0x3884); // turn on rec_en

	writeNMreg(0, NM_PWR_CONFIG_REG, 0x0001); //change LV charge pump from 3:1 to 2:1
	writeNMreg(1, NM_PWR_CONFIG_REG, 0x0001); //change LV charge pump from 3:1 to 2:1
	writeNMreg(0, NM_PWR_CONFIG_REG, 0x0001); //change LV charge pump from 3:1 to 2:1
	writeNMreg(1, NM_PWR_CONFIG_REG, 0x0001); //change LV charge pump from 3:1 to 2:1

	// set first 96 channels to enabled. saves time from enabling them from the GUI
	writeNMreg(0, NM_REC_ENABLE0_REG, 0xFFFF);
	writeNMreg(0, NM_REC_ENABLE1_REG, 0xFFFF);
	writeNMreg(0, NM_REC_ENABLE2_REG, 0xFFFF);
	writeNMreg(0, NM_REC_ENABLE3_REG, 0xFFFF);

	writeNMreg(1, NM_REC_ENABLE0_REG, 0xFFFF);
	writeNMreg(1, NM_REC_ENABLE1_REG, 0xFFFF);
	writeNMreg(1, NM_REC_ENABLE2_REG, 0x0000);
	writeNMreg(1, NM_REC_ENABLE3_REG, 0x0000);

	writeNMreg(0, NM_REC_ENABLE0_REG, 0xFFFF);
	writeNMreg(0, NM_REC_ENABLE1_REG, 0xFFFF);
	writeNMreg(0, NM_REC_ENABLE2_REG, 0xFFFF);
	writeNMreg(0, NM_REC_ENABLE3_REG, 0xFFFF);

	writeNMreg(1, NM_REC_ENABLE0_REG, 0xFFFF);
	writeNMreg(1, NM_REC_ENABLE1_REG, 0xFFFF);
	writeNMreg(1, NM_REC_ENABLE2_REG, 0x0000);
	writeNMreg(1, NM_REC_ENABLE3_REG, 0x0000);

	writeNMreg(0, NM_SYS_CONFIG_REG, 0x4000);
	writeNMreg(0, NM_SYS_CONFIG_REG, 0x4000);
	writeNMreg(1, NM_SYS_CONFIG_REG, 0x4000);
	writeNMreg(1, NM_SYS_CONFIG_REG, 0x4000);

	int data_ack_req = 0;

    for (i=0; i<DATA_BUF_SIZE; i++)
	{
        reg_data_buf[i] = 0;
	}

	for (i = 0; i<NUM_DATA_BUF; i++)
	{
		for (j = 0; j<DATA_BUF_SIZE; j++)
		{
			adc_data_buf[i][j] = 0;
		}
	}

    // put the headers in, since they'll be unchanged
    for (i=0; i<NUM_DATA_BUF; i++)
    {
    	adc_data_buf[i][0] = 0xAA00;
    }

    uint16_t flags, crcflag, crcflag0, crcflag1 = 0;
    int ret, tmp, crcok;
    char buf[5];
	while(1)
	{
		if (BYTES_IN_RINGBUF() >= 5)
		{
			for(i = 0; i < 5; i++)
			{
				buf[i] = ringbuf[rbrdptr];
				rbrdptr = (rbrdptr+1) % (RINGBUF_SIZE);
			}
			if (buf[0] == 0xff)
			{
                data_ack_req = buf[3] & 0x3;

                stream_on = (buf[4] >> 5) & 0x1;
                stream_off = (buf[4] >> 4) & 0x1;
                if (stream_on)
                {
                    stream_en = 1;
                }
                if (stream_off)
                {
                    stream_en = 0;
                    stim_rep = 0;
                }

                artifact_on = (buf[3] >> 7) & 0x1;
                artifact_off = (buf[3] >> 6) & 0x1;
                if (artifact_on)
                {
                    artifact_en = 1;
                    buf_save = 0;
                    buf_good = NUM_DATA_BUF-1;
                    buf_tx = 1;
                }
                if (artifact_off)
                {
                    artifact_en = 0;
                }

                if ((buf[3] >> 5) & 0x01)
                {
                	stim_rep = (buf[1] << 8 | buf[2]) - 1;
                    stim_nm0 = (buf[3] >> 3) & 0x01;
                    stim_nm1 = (buf[3] >> 4) & 0x01;

                    if (stim_nm0)
                    {
                        NMstim(0x09 | (1 << 10), 0);
                        if (stim_rep > 0)
                        {
                            *n0d2 = 0x01 << 16;
                            *n0d1 = OP_READ;
                            stat = *status;
                            stat_flag = true;
                            *status = NM0_START;
                        }
                    }
                    else if (stim_nm1)
                    {
                        NMstim(0x09 | (1 << 10), 1);
                        if (stim_rep > 0)
                        {
                            *n1d2 = 0x01 << 16;
                            *n1d1 = OP_READ;
                            stat = *status;
                            stat_flag = true;
                            *status = NM1_START;
                        }
                    }
                }
			}
			else if ((unsigned char)buf[0] <= 0x4f)
			{
				reg_val = buf[4] + ((uint32_t)buf[3] << 8) + ((uint32_t)buf[2] << 16) + ((uint32_t)buf[1] << 24);
				reg_addr = (reg_val>>16) & 0xFFFF;
				reg_data = reg_val & 0xFFFF;
				*(int*)(REG_BASE+buf[0]) = reg_val;
				while((*status) & (NM0_TXBUSY | NM1_TXBUSY));

				if (rec_enable_write)
				{
					rec_enable_write = false;

					if (buf[0] == 0x24)
					{
						// n1d2, need to check for rec_enable
						if (reg_addr == NM_REC_ENABLE0_REG)
						{
							// channels 64-79
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+64] = 1;
								}
								else
								{
									sel_data_vector[i+64] = 0;
								}
							}
						}
						else if (reg_addr == NM_REC_ENABLE1_REG)
						{
							// channels 80-95
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+80] = 1;
								}
								else
								{
									sel_data_vector[i+80] = 0;
								}
							}
						}
						else if (reg_addr == NM_REC_ENABLE2_REG)
						{
							// channels 96-111
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+96] = 1;
								}
								else
								{
									sel_data_vector[i+96] = 0;
								}
							}
						}
						else if (reg_addr == NM_REC_ENABLE3_REG)
						{
							// channels 112-127
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+112] = 1;
								}
								else
								{
									sel_data_vector[i+112] = 0;
								}
							}
						}
					}

					else if (buf[0] == 0x14)
					{
						// n0d2, need to check for rec_enable
						if (reg_addr == NM_REC_ENABLE0_REG)
						{
							// channels 0-15
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i] = 1;
								}
								else
								{
									sel_data_vector[i] = 0;
								}
							}
						}
						else if (reg_addr == NM_REC_ENABLE1_REG)
						{
							// channels 16-31
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+16] = 1;
								}
								else
								{
									sel_data_vector[i+16] = 0;
								}
							}
						}
						else if (reg_addr == NM_REC_ENABLE2_REG)
						{
							// channels 32-47
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+32] = 1;
								}
								else
								{
									sel_data_vector[i+32] = 0;
								}
							}
						}
						else if (reg_addr == NM_REC_ENABLE3_REG)
						{
							// channels 48-63
							for (i=0; i<16; i++)
							{
								if (((reg_data >> i) & 1) == 1)
								{
									sel_data_vector[i+48] = 1;
								}
								else
								{
									sel_data_vector[i+48] = 0;
								}
							}
						}
					}


				}

				if (((buf[0] == 0x10) || (buf[0] == 0x20)) && (reg_data == 1))
				{
					rec_enable_write = true;
				}
			}
		}
		if (!stat_flag)
		{
			stat = *status;
		}
		stat_flag = false;

		gui_request_counter++;
		// poll SPI slave
		if (gui_request_counter>2000)
		{
			reg_data_buf[0] = 0x0; // header byte indicates no data
			MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
			MSS_SPI_transfer_block( &g_mss_spi0, ((uint8_t *) reg_data_buf) + 1, 2*DATA_BUF_SIZE-1, spi_rx_buf, sizeof(spi_rx_buf));
			MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
			spi_rx_handler();
		}

		if ((stat & NM0_DATA) && (stat & NM1_DATA)) {
			chan = 1;
			stim_flag = false;

			// read in data to temporary buffer, keeping track of stim flag
			// read NM0 adc data
			for(i = 0; i < 66; i++)
			{
				if (i == 0)
				{
					flags = *n0adc;
				}
				else if (i == 65)
				{
					crcflag0 = *n0adc;
				}
				else
				{
					dummy = *n0adc;
					if (sel_data_vector[i-1] != 0)
					{
						if (!stim_flag && (dummy & 0x8000))
							stim_flag = true;
						adc_data_buf[buf_save][chan] = dummy;
						chan++;
					}
				}
			}
//			// read NM1 adc data
//			for(i = 0; i < 66; i++)
//			{
//				if (i == 0)
//					flags = *n1adc;
//				else if (i == 65)
//					crcflag1 = *n1adc;
//				else
//				{
//					dummy = *n1adc;
//					if (sel_data_vector[i+63] != 0)
//					{
//						if (!stim_flag && (dummy & 0x8000))
//							stim_flag = true;
//						adc_data_buf[buf_save][chan] = dummy;
//						chan++;
//					}
//				}
//			}

			// check if we need to do the interpolation
			if (artifact_en)
			{
				if (stim_flag)
				{
					// we have stim, can't do anything yet, just keep track of how many consecutive samples artifact is
					stim_count++;
				}
				else
				{
					// no stim flag, need to check if this is the end of a stim artifact
					if (stim_count > 0)
					{
						// there was previous stim, meaning we now overwrite with interpolated data
						for (chan=1; chan<=NUM_ENABLED_CHANNELS; chan++)
						{
							step = (adc_data_buf[buf_save][chan] - adc_data_buf[buf_good][chan])/(stim_count + 1);
							for (i=1; i<=stim_count; i++)
							{
								buf_idx = (buf_good+i) & (NUM_DATA_BUF-1);
								adc_data_buf[buf_idx][chan] = (adc_data_buf[buf_good][chan] + (step*i)) | 0x8000;
							}
						}
						stim_count = 0;
					}
					buf_good = buf_save;
				}
			}
			//grab accelerometer data
			//bp = accel_sample_read();

//			if ((crcflag0 & 0x1) && (crcflag1 & 0x1) && stream_en)
			if ((crcflag0 & 0x1) && stream_en)
			{
				counter++;	//counter for fake data
				adc_data_buf[buf_tx][DATA_BUF_SIZE-1] = counter;
				adc_data_buf[buf_tx][DATA_BUF_SIZE-2] = counter;
				adc_data_buf[buf_tx][DATA_BUF_SIZE-3] = counter;

				MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
				MSS_SPI_transfer_block( &g_mss_spi0, ((uint8_t *) adc_data_buf[buf_tx]) + 1, 2*DATA_BUF_SIZE-1, spi_rx_buf, sizeof(spi_rx_buf));
				MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
				spi_rx_handler();
			}

			buf_save = (buf_save+1) & (NUM_DATA_BUF-1);
			buf_tx = (buf_tx+1) & (NUM_DATA_BUF-1);
		}

        if (stim_rep > 0) // if we still have a stim train to start
		{
        	if (stat & NM0_ACK) // stim is happening on NM0
			{
        		for(i = 0; i < 4; i++)
				{
					tmp = *n0ack;
					if (i == 1)
						crcok = (tmp == 0x01);
					if (i == 2)
						ret = tmp;
					if (i == 3)
						crcok &= (tmp & 1);
				}
        		if (crcok & !(ret & (1 << 15))) // previous stim ended, so start a new train
        		{
        			stim_rep--;
        			NMstim(0x09 | (1 << 10), 0);
        			if (stim_rep > 0)
        			{
        				*n0d2 = 0x01 << 16;
						*n0d1 = OP_READ;
						stat = *status;
						stat_flag = true;
						*status = NM0_START;
        			}
        		}
        		else
        		{
        			*n0d2 = 0x01 << 16;
					*n0d1 = OP_READ;
					stat = *status;
					stat_flag = true;
					*status = NM0_START;
        		}
			}
            else if (stat & NM1_ACK) // stim is happening on NM1
            {
                for(i = 0; i < 4; i++)
                {
                    tmp = *n1ack;
                    if (i == 1)
                        crcok = (tmp == 0x01);
                    if (i == 2)
                        ret = tmp;
                    if (i == 3)
                        crcok &= (tmp & 1);
                }
                if (crcok & !(ret & (1 << 15))) // previous stim ended, so start a new train
                {
                    stim_rep--;
                    NMstim(0x09 | (1 << 10), 1);
                    if (stim_rep > 0)
                    {
                        *n1d2 = 0x01 << 16;
                        *n1d1 = OP_READ;
                        stat = *status;
                        stat_flag = true;
                        *status = NM1_START;
                    }
                }
                else
                {
                    *n1d2 = 0x01 << 16;
                    *n1d1 = OP_READ;
                    stat = *status;
                    stat_flag = true;
                    *status = NM1_START;
                }
            }
        }
        else // not doing stim, so continue with register operations
        {
            if (stat & NM0_ACK)
			{
				reg_data_buf[0] = 0xFF00; // header byte
				for(i = 0; i < 4; i++)
				{
					if (i == 0)
						flags = *n0ack;
					else if (i == 3)
						crcflag = *n0ack;
					else
						reg_data_buf[i] = *n0ack;
				}
				if (crcflag & 0x1)
				{
					if (data_ack_req & 0x01)
					{
						MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
						MSS_SPI_transfer_block( &g_mss_spi0, ((uint8_t *) reg_data_buf) + 1, 2*DATA_BUF_SIZE-1, spi_rx_buf, sizeof(spi_rx_buf) );
						MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
						spi_rx_handler();
						data_ack_req &= ~0x01;
					}
				}
			}

			if (stat & NM1_ACK)
			{
				reg_data_buf[0] = 0xFF00; // header byte
				for(i = 0; i < 4; i++)
				{
					if (i == 0)
						flags = *n1ack;
					else if (i == 3)
						crcflag = *n1ack;
					else
						reg_data_buf[i] = *n1ack;
				}
				if (crcflag & 0x1)
				{
					if (data_ack_req & 0x02)
					{
						MSS_SPI_set_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
						MSS_SPI_transfer_block( &g_mss_spi0, ((uint8_t *) reg_data_buf) + 1, 2*DATA_BUF_SIZE-1, spi_rx_buf, sizeof(spi_rx_buf) );
						MSS_SPI_clear_slave_select( &g_mss_spi0, MSS_SPI_SLAVE_0 );
						spi_rx_handler();
						data_ack_req &= ~0x02;
					}
				}
			}
        }
	}

	return 0;
}

void spi_rx_handler()
{
	// check if there is meaningful data on SPI rx (indicated by non-zero header byte)
	// if so, throw it in ring buffer.
	gui_request_counter=0;
	int i;
	if (spi_rx_buf[0] == 0xAA) {
 		for(i = 0; i < 5; i++)
		{
			if ((rbwrptr+1)%(RINGBUF_SIZE) != rbrdptr)
			{
				ringbuf[rbwrptr] = spi_rx_buf[i+1];
				rbwrptr = (rbwrptr+1) % (RINGBUF_SIZE);
			}
		}
	}
}

void GPIO5_IRQHandler()
{
	// The MPU6050 data ready signal triggers this interrupt.
	// Since the only flags available are data ready and FIFO overflow we set a timer to emulate a "mostly full" flag.
	MSS_GPIO_clear_irq(MSS_GPIO_5);
	// When this timer expires, Timer1_IRQHandler() sets a flag. In while(1) if the flag is set we check the MPU6050 FIFO.
	MSS_TIM1_start();
	MSS_TIM1_load_immediate(MPU6050_FIFO_DELAY);
}

void Timer1_IRQHandler()
{
	MSS_TIM1_clear_irq();
	mpu6050_irq_active = 1;
}
