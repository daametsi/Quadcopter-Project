// Header file for Camera Project Prototype
// daametsi

#include <pthread.h>
#include "Ov7670.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <curses.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <poll.h>
#include <signal.h>
#include "SimpleGPIO.h"
#include "GpioIntf.h"
#include "GPIO_CONST.h"

// Function prototypes
void cameraReadImageStart();
void cameraReadImageStop();
void* updateOsc(void* args);
void* getUART(void* argus);
int setPWM(int gpioBank, int setPin);


typedef struct OscThread{
	pthread_t tid;
	int setPin;
	int gpioBank;
} OscThread;

typedef struct UARTThread{
	pthread_t tid;
	char c;
} UARTThread;


 /****************************************************************
 * Constants
 ****************************************************************/
 
#undef DEBUG
#define GPIO_DIR "/sys/class/gpio"

#define null NULL

#define MAX_SERIAL_BUFFER_SIZE 7
/* change this definition for the correct port */
#define MODEMDEVICE_I2C "/dev/i2c-0"

#define FREQ_KHZ				1


#define LED_PIN					116		//P9_41
