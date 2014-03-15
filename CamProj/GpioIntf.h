
#ifndef GPIOINTF_H_
#define GPIOINTF_H_

class GpioIntf {

	public:
		GpioIntf();
		GpioIntf(int baseAddr);		
		int getBit(int pin);
		int setBit(int pin, int val);
		int setDirection(int pin, int dir);
	private:
		char* baseAddr;
		// I/O access
		volatile unsigned *gpio;
		// Get direction control register contents
    	volatile unsigned *controlRegisterDirection;
		// 
		volatile unsigned *controlRegisterIN;
		// 
		volatile unsigned *controlRegisterOUT;
};
#endif 
