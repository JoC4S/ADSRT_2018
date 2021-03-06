/*
 *  S.Bernadas 2018
 *  Raspberry Pi I2C interface demo for ADC ADS1015
 *  The board attached is Automation HAT
 * 
 *  sudo raspi-config --> advanced options --> enable i2c
 *
 *  sudo apt-get install libi2c-dev i2c-tools
 *
 *  Then build with:
 *
 *       gcc -Wall ADC_AutmHAT_I2c.c -o ADC_AutmHAT_I2c
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEV_ID 0x48
#define DEV_PATH "/dev/i2c-1"
#define CNF_REG 0x01
#define CNV_REG 0x00

void delay(int);

int main(void)
{
    int fd = 0;
	uint8_t adc_l=0, adc_h=0;
    uint16_t adc_out = 0;
    int adc_ok=0;
    double adc_v = 0.0;

    /* open i2c comms */
    if ((fd = open(DEV_PATH, O_RDWR)) < 0) {
	perror("Unable to open i2c device");
	exit(1);
    }

    /* configure i2c slave */
    if (ioctl(fd, I2C_SLAVE, DEV_ID) < 0) {
	perror("Unable to configure i2c slave device");
	close(fd);
	exit(1);
    }

    /* Run one-shot measurement (AIN1-gnd), FSR +-4.096V, 160 SPS, 
     * no windows, no comparation */
    // LowSByte MSByte  they are inverted
    i2c_smbus_write_word_data(fd, CNF_REG, 0x83D3);

    /* Wait until the measurement is complete */
	usleep(700);		/* 700 microseconds */
	adc_out = i2c_smbus_read_word_data(fd, CNV_REG);
	// swap bytes
	adc_l=adc_out >> 8;
	adc_h=adc_out;
	adc_ok=(adc_h<<8)|adc_l;
	// dichart 4 LSB
	adc_ok=(adc_ok>>4);
	
    printf("Value ADC = %d \n",adc_out);
    printf("Value ADC = %x \n",adc_out);
    printf("Value L   = %x \n",adc_l);
    printf("Value H   = %x \n",adc_h);
    printf("Value OK  = %x \n",adc_ok);
    
    /* calculate output values */
    adc_v = 4.095 * (adc_ok / 2047.0);

    /*output */
    printf("Value ADC in V = %.2fV\n", adc_v);
    printf("Value input in V = %.2fV\n", adc_v*47/6);


    close(fd);

    return (0);
}
