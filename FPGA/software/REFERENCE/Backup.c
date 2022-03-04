//NIOS Imports
#include "system.h"
#include "altera_up_avalon_accelerometer_spi.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"

//Standard Imports
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

//Time Import
#include "sys/alt_timestamp.h"
#define SAMPLING_TIME 3
#define INTERVALSECOND 100000000


//Variable Declerations
//UART
FILE* fp;
char dataIn[];

//LED
alt_u8 led;
int level;

//Timer
alt_u32 startTime, stopTime;

//Accelerometer setup
alt_32 x_read;
alt_32 y_read;
alt_32 z_read;
alt_up_accelerometer_spi_dev * acc_dev;

void led_write(alt_u8 led_pattern) {
    IOWR(LED_BASE, 0, led_pattern);
}

void processData(char* data) {
	printf("Data recieved: %s\n", data);
}


int main () {
	acc_dev = alt_up_accelerometer_spi_open_dev("/dev/accelerometer_spi");
	if (acc_dev == NULL) {
		// if return 1, check if the spi ip name is "accelerometer_spi"
		return 1;
	}
	//Open file for reading and writing, non blocking
	fp = open("/dev/jtag_uart", O_RDWR|O_NONBLOCK|O_NOCTTY|O_SYNC);
	if (fp)	{
		//Begin
		led_write(0x7);
		alt_timestamp_start();
		startTime = alt_timestamp();
		// Loop until KILL command
		while (dataIn != 'k') {
			// Get data/command from Python interface
			led_write(0x60);
			//dataIn = getc(fp);
			if(read(fp, &dataIn, 1) > 0) {
				processData(dataIn);
			}
			//If file write fails
			if (ferror(fp)) {// Check if an error occurred with the file
				clearerr(fp);// If so, clear it.
			}
			//Obtain values at a certain frequency
			stopTime = alt_timestamp();
			//Frequency of accelerometer is 2^SAMPLING_TIME Hz, with 6, 64Hz
			if ((stopTime-startTime) > (INTERVALSECOND >> SAMPLING_TIME-1)) {
				alt_up_accelerometer_spi_read_x_axis(acc_dev, & x_read);
				alt_up_accelerometer_spi_read_y_axis(acc_dev, & y_read);
				alt_up_accelerometer_spi_read_z_axis(acc_dev, & z_read);
				printf("%d %d %d\n", x_read, y_read, z_read);
				startTime = alt_timestamp();
			}
		}
		printf("Killed.\n");
		led_write(0x1);
		fclose (fp);
	}
	return 0;
}
