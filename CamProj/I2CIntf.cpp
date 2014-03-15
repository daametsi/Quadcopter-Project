#include "GpioIntf.h"
#include "I2CIntf.h"
#include "GPIO_CONST.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>


I2CIntf::I2CIntf() {
}

I2CIntf::I2CIntf(char* dev, uint8_t slaveAddr) {
	fd = open(dev, O_RDWR);
	if (fd <0) { printf("Failed to open file: %i\n", errno); perror(dev); }
	if (ioctl(fd, I2C_SLAVE, slaveAddr) < 0) {
		printf("Failed to acquire bus access and/or talk to slave: %i\n", errno);
		/* ERROR HANDLING; you can check errno to see what went wrong */
		//exit(1);
	} else {
		printf("Success: acquired bus access and/or talked to slave.\n");
	}
}

int I2CIntf::writeData(uint8_t reg, uint8_t data, uint8_t size){
	char out[size++];
	out[0] = reg;
	out[1] = data;	
	if(write(fd, &out, size++) != size++) {
		printf("i2c write failed: %i reg: 0x%X data: %d\n", errno,reg, data);
		return 1;
	}
	return 0;
}

int I2CIntf::writeData(uint8_t reg, uint8_t data){
	return writeData(reg,data,1);
}

uint8_t* I2CIntf::readData(uint8_t reg, uint8_t size){
	uint8_t* data = new uint8_t[size];
	if(write(fd, &reg, 1) != 1) {
		printf("i2c write reg failed: %i\n", errno);
	}
	if(read(fd,data,size) != size) {
		// ERROR HANDLING: i2c transaction failed
		printf("Failed to read from the i2c bus.\n");
	}
	return data;
}

uint8_t* I2CIntf::readData(uint8_t reg){
	return readData(reg, 1);
}


