#include "camProj.h"

char buf1[1];
char bufResponse[MAX_SERIAL_BUFFER_SIZE];
int fd1, fd2, res1, rightDuty, leftDuty;

Ov7670 camera;
uint8_t camera_init_success = false;


/****************************************************************
 * set duty cycle for PWM
 ****************************************************************/
int setPWM(int gpioBank, int setPin){
	//while(1){
		if(gpioBank = 0){
			//while(1){
				camera.gpio0->setBit(BIT_RCK,1);
				//usleep(10);
				camera.gpio0->setBit(BIT_RCK,0);
			//}
		} else  {
			//while(1){
				camera.gpio2->setBit(setPin,HIGH);
				//usleep(10);
				camera.gpio2->setBit(setPin,LOW);
			//}
		}
	//}
	return 1;
}

/****************************************************************
 * Intermediary function since you must call void* to use pthread
 ****************************************************************/
void* updateOsc(void* argus){
	OscThread* args = (OscThread*) argus;
	while(1){
	setPWM(args->gpioBank,args->setPin);
	}
	return null;
}

void* getUART(void* argus){
	UARTThread* args = (UARTThread*) argus;
	//listenForCommands();
	return null;
}

void cameraReadImageStart() {
	#ifdef USE_TFT
	linesRead = 0;
	#endif
	#ifdef USE_SD
	file.open(image.nrToPictureString(0, snapCounter++), O_WRITE | O_CREAT | O_TRUNC);
	#endif
}

void cameraReadImageStop() {
	#ifdef USE_SD
	file.close();
	eeprom_write_word(&EEPROMSnapCounter, snapCounter);
	#endif
}

int main(int argc, char** argv)
{
	gpio_export(LED_PIN);
	gpio_set_dir(LED_PIN, OUTPUT_PIN);
	gpio_set_value(LED_PIN, HIGH);

	/*OscThread OscRCK;
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	OscRCK.gpioBank = 0;
	OscRCK.setPin = BIT_RCK;
	pthread_create(&tid, &attr, &updateOsc, &OscRCK);*/

	//OscThread OscVSYNC;
	/*OscVSYNC.gpioBank = 2;
	OscVSYNC.setPin = BIT_VSYNC;
	pthread_create(&tid, &attr, &updateOsc, &OscVSYNC);*/

	camera.readImageStartFunctionPtr = &cameraReadImageStart;
	camera.readImageStopFunctionPtr = &cameraReadImageStop;

	camera.init();
	camera.brightness(2);
	camera_init_success = camera.init_success;
	printf("Camera initialized: %d\n",camera_init_success);

	while(1)
	{

		// Read pin
		//printf("databus value: %d\n",getData());
		//camera.gpio0->setBit(BIT_PWDN, 0);
		//camera.gpio0->setBit(BIT_OE, 0);
		// Set VSYNC
		//camera.gpio0->setBit(BIT_OE,HIGH);

		// Clear VSYNC
		//camera.gpio0->setBit(BIT_OE,LOW);
		camera.gpio2->setBit(BIT_D06, HIGH);

	}
	return 0;
}
