#include <stdint.h>

class I2CIntf {
public:
	I2CIntf();
	I2CIntf(char* dev, uint8_t slaveAddr);
	uint8_t* readData(uint8_t reg);
	int writeData(uint8_t reg, uint8_t data);
	uint8_t* readData(uint8_t reg, uint8_t size);
	int writeData(uint8_t reg, uint8_t data, uint8_t size);
private:
	int fd;
};
