#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <curses.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <linux/i2c-dev.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "SimpleGPIO.h"
#include "ChipCapture.h"
#include "accelReader.h"

/****************************************************************
 * Constants
 ****************************************************************/
 
#undef DEBUG
#define GPIO_DIR "/sys/class/gpio"
#define AIN_DIR "/sys/devices/platform/omap/tsc"
#define null NULL

char buf[10];
char buf1[1];
char bufResponse[MAX_SERIAL_BUFFER_SIZE];
int fd1, fd2, res1;


FILE *fp;

int initializeI2C(){
	int addr;
	char out[2];
	fd2 = open(MODEMDEVICE_I2C, O_RDWR);
        if (fd2 <0) {perror(MODEMDEVICE_I2C); exit(-1); }
	addr = 0b1101000;
	if (ioctl(fd2, I2C_SLAVE, addr) < 0) {
		printf("Failed to acquire bus access and/or talk to slave.\n");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}
	out[0] = 0x6B;
	out[1] = 0x21;
	if(write(fd2, &out, 2) != 2) {
		printf("i2c write failed: %i\n", errno);
	}
	out[0] = 0x6C;
	out[1] = 0x80;
	if(write(fd2, &out, 2) != 2) {
		printf("i2c write failed: %i\n", errno);
	}
	return 1;
}

	
int main(int argc, char** argv)
{
	char* buffer;
	initializeI2C();
	gpio_export(LED_PIN);
        gpio_set_dir(LED_PIN, OUTPUT_PIN);
        gpio_set_value(LED_PIN, HIGH);

	//printf("listening for commands.. %c", c);
	//sendInt(c);
	//listenForCommands();

	/*PWMThread lPWM;
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, &updatePWM, &lPWM);
	lPWM.toggle = 0;
	lPWM.activePin = 0;
	PWMThread rPWM;
	pthread_create(&tid, &attr, &updatePWM, &rPWM);
        rPWM.toggle = 0;
	rPWM.activePin = 0;
	UARTThread uartScan;
	pthread_create(&tid, &attr, &getUART, &uartScan);*/

	while(1)
	{
		int[] accel;
		accel = getAccel();
		usleep(100000);
	}
	return 0;
}
