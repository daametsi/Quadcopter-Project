#include <pthread.h>

// Function prototypes
int initializeUARTS();
int initializeI2C();
int sendInt(int value);
/*void* getAccelAxis(void* args);
int getAccel(int Dir);


typedef struct getAccel{
	pthread_t tid;
	int Dir;
} getAccel;*/

//UART Settings
#define BAUDRATE_UART2 B9600
#define BAUDRATE_UART4 B9600
#define MAX_SERIAL_BUFFER_SIZE 7
/* change this definition for the correct port */
#define MODEMDEVICE_I2C "/dev/i2c-1"

#define FREQ_KHZ				1


#define UART_SELECT_0_GPIO_PIN_NUM 		44		//P8_11
#define UART_SELECT_1_GPIO_PIN_NUM 		45		//P8_12
#define UART_SELECT_2_GPIO_PIN_NUM 		46		//P8_14
#define UART_SELECT_3_GPIO_PIN_NUM              47              //P8_15
#define LED_PIN					116		//P9_41
