/*
 *  S Bernadas 2018
 *  Raspberry Pi I2C interface demo for all LEDs attached to 
 *  SN3218 18 channel LED driver I2C Device
 *  The board attached is Automation HAT
 * 
 *  sudo raspi-config --> advanced options --> enable i2c
 *
 *  sudo apt-get install libi2c-dev i2c-tools
 *
 *  Then build with:
 *
 *       gcc -Wall LEDs_AutmHAT_I2c.c -o LEDs_AutmHAT_I2c
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEV_ID 0x54
#define DEV_PATH "/dev/i2c-1"
#define SHTDWN_REG 0x00
#define CTRL_REG1 0x13
#define CTRL_REG2 0x14
#define CTRL_REG3 0x15
#define PWM_UPDATE_REG 0x16
#define RST_REG 0x17

int main(void)
{
    int fd = 0;

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

    /* Power down the device (clean start) */
    i2c_smbus_write_byte_data(fd, RST_REG, 0x00);

    /* Turn on the device in normal operation  */
    i2c_smbus_write_byte_data(fd, SHTDWN_REG, 0x01);

    /* Activate LEDs 1-3 */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x07);

    /* SET the PWM value for LEDs 1 to 3 */
    i2c_smbus_write_byte_data(fd, 0x01, 0x0F); // LED1 -> 015/255
    i2c_smbus_write_byte_data(fd, 0x02, 0x02); // LED2 -> 002/255
    i2c_smbus_write_byte_data(fd, 0x03, 0x80); // LED3 -> 128/255
	/* Values stored in a temporary register */
	
	/* Update values of registers*/
	i2c_smbus_write_byte_data(fd, PWM_UPDATE_REG, 0x00); //write any value
	
	/* wait fo 1 second*/
	usleep(1000000);
	
    /* Reset & Power down the device  */
    i2c_smbus_write_byte_data(fd, RST_REG, 0x00);
    i2c_smbus_write_byte_data(fd, SHTDWN_REG, 0x00);
	
    
    close(fd);

    return (0);
}
