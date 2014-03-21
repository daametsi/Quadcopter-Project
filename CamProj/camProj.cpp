#include "camProj.h"

char buf1[1];
char bufResponse[MAX_SERIAL_BUFFER_SIZE];
int fd1, fd2, res1, rightDuty, leftDuty;

Ov7670 camera;
Image image(&camera);
ofstream file;
uint8_t camera_init_success = false;
volatile uint16_t snapCounter = 0;

volatile uint8_t vsyncCounter = 0;
volatile uint32_t runtime = 0;
volatile bool oneSecondPassed = false;
volatile bool takePictures = false;


/****************************************************************
 * Handler for verticle(y direction)
 ****************************************************************/
void vsync_handler() {
	//printf("main: handling vsync\n");
	camera.vsync_handler();
	vsyncCounter++;
	if (vsyncCounter > 15) { // with qqvga fps = 15
		vsyncCounter = 0;
		runtime++;
		oneSecondPassed = true;
	}
}

/****************************************************************
 * Handler for horizontal(x direction)
 ****************************************************************/
void href_handler() {
	camera.href_handler();
}


/****************************************************************
 * set duty cycle for PWM
 ****************************************************************/
int setPWM(int gpioBank, int setPin){
	//camera.gpio0->setBit(BIT_OE, 0);
	//camera.gpio0->setBit(BIT_WE, 0);
	//camera.gpio0->setBit(BIT_RCK,1);
	//camera.gpio0->setBit(BIT_RCK,0);
	/*while(1){
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
	}*/
	return 1;
}

/****************************************************************
 * Print status of pins
 ****************************************************************/
int printPinState(){
	//cameraReadImageStart();
	//printf("databus value: %d\r",camera.getData());
	printf("HREF: %d", camera.gpio2->getBit(BIT_HREF));
	//printf("RRST: %d WRST: %d OE: %d, WE: %d RCK: %d, STR: %d PWDN: %d, RST: %d D00: %d, D01: %d D02: %d, D03: %d D04: %d, D05: %d D06: %d, D07: %d HREF: %d, VSYNC: %d\r", camera.gpio2->getBit(BIT_RRST),camera.gpio2->getBit(BIT_WRST), camera.gpio0->getBit(BIT_OE),camera.gpio0->getBit(BIT_WE), camera.gpio0->getBit(BIT_RCK),camera.gpio2->getBit(BIT_STR), camera.gpio0->getBit(BIT_PWDN),camera.gpio2->getBit(BIT_RST), camera.gpio2->getBit(BIT_D00),camera.gpio2->getBit(BIT_D01), camera.gpio2->getBit(BIT_D02),camera.gpio2->getBit(BIT_D03), camera.gpio2->getBit(BIT_D04),camera.gpio2->getBit(BIT_D05), camera.gpio2->getBit(BIT_D06),camera.gpio2->getBit(BIT_D07), camera.gpio2->getBit(BIT_HREF),camera.gpio2->getBit(BIT_VSYNC));
	return 1;
}




/****************************************************************
 * Intermediary function since you must call void* to use pthread
 ****************************************************************/
void* getPinState(void* argus){
	PinState* args = (PinState*) argus;
	while(1){
		printPinState();
		usleep(1000);
	}
	return null;
}

void* updateOsc(void* argus){
	while(1){
		camera.gpio0->setBit(BIT_RCK,1);
		camera.gpio0->setBit(BIT_RCK,0);
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
	//#ifdef USE_SD
	//printf("read image start\n");
	file.open(image.nrToPictureString(0, snapCounter++), ios::out | ios::binary);
	//#endif
}

void cameraReadImageStop() {
	#ifdef USE_SD
	file.close();
	//eeprom_write_word(&EEPROMSnapCounter, snapCounter);
	#endif
}

void cameraBufferFull(uint8_t * buffer) {
	printf("cam buffer full\n");
	//#ifdef USE_SD
	file.write( (char*) buffer, camera.BUFFER_SIZE);
	//#endif

	/*#ifdef USE_TFT
	uint8_t x = 0;
	for (uint16_t i = 0; i<camera.BUFFER_SIZE; i+=3) {
		screen.drawPixel(x++, linesRead, screen.newColor(buffer[i], buffer[i+1], buffer[i+2]));
	}
	linesRead++;
	#else
	for (uint16_t i = 0; i<camera.BUFFER_SIZE; i+=3) {
		Serial << _BYTE(buffer[i]) << _BYTE(buffer[i+1]) << _BYTE(buffer[i+2]);
	}
	#endif*/
}

void loop() {
	#ifdef SERIAL_INFO
	checkSerialInput();
	#endif

	if (oneSecondPassed) {
		oneSecondPassed = false;
		if (takePictures) {
			if (runtime % 10 == 0) {
				#ifdef SERIAL_DEBUG
				serialTimeStamp();
				Serial << F("cap - ") << snapCounter << endll;
				#endif
				camera.capture_image();
			}
		}
	}
}


int main(int argc, char** argv)
{
	gpio_export(LED_PIN);
	gpio_set_dir(LED_PIN, OUTPUT_PIN);
	gpio_set_value(LED_PIN, HIGH);

	PinState StatePrinter;
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, &getPinState, &StatePrinter);

	OscThread OscRCK;
	OscRCK.gpioBank = 0;
	OscRCK.setPin = BIT_RCK;
	pthread_create(&tid, &attr, &updateOsc, &OscRCK);

	//OscThread OscVSYNC;
	/*OscVSYNC.gpioBank = 2;
	OscVSYNC.setPin = BIT_VSYNC;
	pthread_create(&tid, &attr, &updateOsc, &OscVSYNC);*/

	camera.bufferFullFunctionPtr = &cameraBufferFull;
	camera.readImageStartFunctionPtr = &cameraReadImageStart;
	camera.readImageStopFunctionPtr = &cameraReadImageStop;

	camera.init();
	camera.brightness(2);
	camera_init_success = camera.init_success;
	printf("Camera initialized: %d\n",camera_init_success);

	while(1)
	{
		//if(camera.gpio2->getBit(BIT_VSYNC))
		//	vsync_handler();
		// Read pin
		//printf("databus value: %d\n",getData());
		//camera.gpio0->setBit(BIT_PWDN, 0);
		//camera.gpio0->setBit(BIT_RCK, 0);
		// Set RCK
		//camera.gpio0->setBit(BIT_RCK,HIGH);

		// Clear RCK
		//camera.gpio2->setBit(BIT_RCK,LOW);
		//camera.gpio2->setBit(BIT_D06, HIGH);
	}
	return 0;
}
