#include "GpioIntf.h"
#include "GPIO_CONST.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

GpioIntf::GpioIntf() {
}

GpioIntf::GpioIntf(int baseAddr) {
	int mem_fd;
	char *gpio_mem, *gpio_map;
	/* open /dev/mem */
	if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
		printf("can't open /dev/mem \n");
		//exit (-1);
	}
	
	gpio_map = (char *)mmap(
		0,
		GPIO_SIZE,
		PROT_READ|PROT_WRITE,
		MAP_SHARED,
		mem_fd,
		baseAddr
    );
	if (gpio_map == MAP_FAILED) {
            printf("mmap error %d\n", (int)gpio_map);
            //exit (-1);
    }

    // Always use the volatile pointer!
    gpio = (volatile unsigned *)gpio_map;
	//
	controlRegisterDirection = (volatile unsigned *) (gpio + GPIO_OE);
	// Gpio in
	controlRegisterIN = (volatile unsigned *) (gpio + GPIO_IN);
	// Gpio out
	controlRegisterOUT = (volatile unsigned *) (gpio + GPIO_OUT);
	close(mem_fd);
}

int GpioIntf::getBit(int pin){	
	//printf("pin: %d val: %d\n",pin, (*(controlRegisterIN) >> pin) & 1);
	return (*(controlRegisterIN) >> pin) & 1;
}

int GpioIntf::setBit(int pin, int val){	
	if(val == 1){	
		*(controlRegisterOUT) = *(controlRegisterOUT) | (1 << pin);
	} else {
		*(controlRegisterOUT) = *(controlRegisterOUT) & (~(1 << pin));
	}
}

int GpioIntf::setDirection(int pin, int dir){
	setBit(pin, 0);
	if(dir == GPIO_DIR_OUT){
    	// Set outputs
    	*(controlRegisterDirection) = *(controlRegisterDirection) & (~(1 << pin));
    } else {
    	// Set inputs
    	*(controlRegisterDirection) = *(controlRegisterDirection) | (1 << pin);
	}
}
