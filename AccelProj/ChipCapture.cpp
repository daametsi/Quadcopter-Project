#include "ChipCapture.h"

ChipCapture::ChipCapture() {
}

int[] ChipCapture::getAccel() {	
	int* aBuf;
	int* data = new int[3];
	int i;
	int numBytes = 6;
	aBuf = getData(0x3B, numBytes);
	for(i = 0; i < 3; i++){
		data[i] = aBuf[2*i+1];
		data[i] = data[i] << 8;
		data[i] =+ aBuf[2*i];
	}
	return data;
}

int[] ChipCapture::getGyro() {
	buf[0] = 0x43; //gyro
}

int[] ChipCapture::getData(char reg, int numBytes){	
	int* buf = new int[numBytes];
	if (write(fd2,buf,1) != 1) {
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to the i2c bus.\n");
	}	
	if (read(fd2,buf,numBytes) != numBytes) {
		// ERROR HANDLING: i2c transaction failed
		printf("Failed to read from the i2c bus.\n");
	}
	return buf;
}
