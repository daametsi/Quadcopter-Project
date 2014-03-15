/*
 * Ov7670.cpp
 *
 *  Created on: Aug 26, 2013
 *      Author: arndtjenssen
 */

#include "Ov7670.h"


Ov7670::Ov7670() {
	capture_request = false;
	capture_done = true;
	busy = false;
	init_success = false;
	line_counter = 0;
	last_line_counter = 0;
	camera_mode = MODE_YUV;
	bufferPos = 0;
	bufferFullFunctionPtr = 0;
	readImageStartFunctionPtr = 0;
	readImageStopFunctionPtr = 0;
	edgeEnhacementEnabled = false;
	denoiseEnabled = false;
	// setup i/o ports and directions	
	io_setup();
	
	// initialize new I2C camera interface
	camI2CIntf = new I2CIntf(MODEMDEVICE_I2C_CAM,OV7670_I2C_ADDR);

	// initialize camera for reading
	gpio0->setBit(BIT_PWDN, 0);
	gpio0->setBit(BIT_OE, 0);

	// reseting..
	gpio2->setBit(BIT_RST, 0);
	usleep(1000*200);
	gpio2->setBit(BIT_RST, 1);
	usleep(1000*200);
}

void Ov7670::nightMode(bool enable) {
	if (enable) {
		camI2CIntf->writeData(REG_COM11, COM11_EXP|COM11_HZAUTO);
		camI2CIntf->writeData(REG_COM11, COM11_EXP|COM11_HZAUTO|COM11_NIGHT|COM11_NIGHT_FR8);
	} else {
		camI2CIntf->writeData(REG_COM11, COM11_EXP|COM11_HZAUTO|COM11_NIGHT|COM11_NIGHT_FR8);
		camI2CIntf->writeData(REG_COM11, COM11_EXP|COM11_HZAUTO);
	}
}

// -2 (low contrast ) to +2 (high contrast)
void Ov7670::contrast(int8_t value) {
	static const uint8_t values[] = {0x60, 0x50, 0x40, 0x38, 0x30};

	value = min(max((value + 2), 0), 4);
	camI2CIntf->writeData(REG_CONTRAST, values[value]);
}

// -2 (dark) to +2 (bright)
void Ov7670::brightness(int8_t value) {
	static const uint8_t values[] = {0xb0, 0x98, 0x00, 0x18, 0x30};

	value = min(max((value + 2), 0), 4);
	camI2CIntf->writeData(REG_BRIGHT, values[value]);
}

// 0 - Normal, 1 - Antique, 2 - Bluish, 3 - Greenish
// 4 - Redish, 5 - B&W, 6 - Negative, 7 - B&W negative
void Ov7670::specialEffect(uint8_t value) {
	value = min(max((value + 2), 0), 4);
	transfer_regvals(ov7670_effects[value]);
}

// 0 to disable, > 0 enable and set edge enhancement factor
void Ov7670::edgeEnhancement(uint8_t value) {
	uint8_t v = COM16_AWBGAIN | (denoiseEnabled ? COM16_DENOISE : 0);

	if (value == 0) {
		camI2CIntf->writeData(REG_COM16, v);
		camI2CIntf->writeData(REG_EDGE, 0);
	} else {
		camI2CIntf->writeData(REG_COM16, v | COM16_EDGE);
		camI2CIntf->writeData(REG_EDGE, value);
	}
	edgeEnhacementEnabled = (value > 0);
}

// 0 to disable, > 0 enable and set denoise factor
void Ov7670::denoise(uint8_t value) {
	uint8_t v = COM16_AWBGAIN | (edgeEnhacementEnabled ? COM16_EDGE : 0);

	if (value == 0) {
		camI2CIntf->writeData(REG_COM16, v);
		camI2CIntf->writeData(REG_EDGE, 0);
	} else {
		camI2CIntf->writeData(REG_COM16, v | COM16_DENOISE);
		camI2CIntf->writeData(REG_DENOISE_STRENGTH, value);
	}
	denoiseEnabled = (value > 0);
}


void Ov7670::init() {

	//if (camI2CIntf->readData(REG_PID) != 0x76) {
	//    printf("PANIC! REG_PID != 0x76!\n");
	//    while (1);
	//}
	//printf("%X\n", (unsigned int) camI2CIntf->readData(0x09));
	camI2CIntf->writeData(REG_COM7, 0x80); /* reset to default values */
	//camI2CIntf->writeData(REG_CLKRC, 0x80);
	//camI2CIntf->writeData(REG_COM11, 0x0A);
	//camI2CIntf->writeData(REG_TSLB, 0x04);
	//camI2CIntf->writeData(REG_TSLB, 0x04);
	
	// set MODE to YUV
	//init_success = reset(MODE_YUV);
}

/**
 * returns 1 if camera was initialized succesful
 * mode: MODE_RGB444, MODE_RGB555, MODE_RGB565, MODE_YUV
 */
uint8_t Ov7670::reset(uint8_t mode) {
	uint8_t ret = 0;
	camera_mode = mode;

	init_camera_reset();

	switch (camera_mode) {
	case MODE_RGB444:
		ret = init_rgb444_qqvga();
		if (ret != 1) return ret;
		break;
	case MODE_RGB555:
		ret = init_rgb555_qqvga();
		if (ret != 1) return ret;
		break;
	case MODE_RGB565:
		ret = init_rgb565_qqvga();
		if (ret != 1) return ret;
		break;
	case MODE_YUV:
		ret = init_yuv_qqvga();
		if (ret != 1) return ret;
		break;

	}

	init_negative_vsync();
	ret = init_default_values();

	return ret;
}

void Ov7670::vsync_handler() {
	if (capture_request) {
		WRITE_RESET;
		WRITE_ENABLE;
		capture_request = false;
		capture_done = false;
	} else {
		WRITE_DISABLE;
		WRITE_RESET;
		if (busy) {
			busy = false;
			capture_done = true;
		}
	}
	//last_line_counter = line_counter;
	//line_counter = 0;
}

void Ov7670::href_handler() {
	line_counter++;
}

void Ov7670::capture_next() {
	capture_request = true;
	busy = true;
}

uint8_t Ov7670::captured() {
	uint8_t result;
   if (busy) {
       result = false ;
   } else {
       result = capture_done;
       capture_done = false;
   }
   return result ;
}

uint8_t Ov7670::getData(){
	return	gpio2->getBit(BIT_D00) + 
			(gpio2->getBit(BIT_D01) << 1) +
			(gpio2->getBit(BIT_D02) << 2) +
			(gpio2->getBit(BIT_D03) << 3) +
			(gpio2->getBit(BIT_D04) << 4) +
			(gpio2->getBit(BIT_D05) << 5) +
			(gpio2->getBit(BIT_D06) << 6) +
			(gpio2->getBit(BIT_D07) << 7);
}

void Ov7670::io_setup()
{
    // Enable all GPIO banks
    // Without this, access to deactivated banks (i.e. those with no clock source set up) will (logically) fail with SIGBUS
    // Idea taken from https://groups.google.com/forum/#!msg/beagleboard/OYFp4EXawiI/Mq6s3sg14HoJ
    system("echo 5 > /sys/class/gpio/export");
    system("echo 65 > /sys/class/gpio/export");
    system("echo 105 > /sys/class/gpio/export");
	
	gpio0 = new GpioIntf(GPIO0_BASE);
	gpio0->setDirection(BIT_OE,GPIO_DIR_OUT); //AL422B_FIFO
	gpio0->setDirection(BIT_WE,GPIO_DIR_OUT); //AL422B_FIFO
	gpio0->setDirection(BIT_RCK,GPIO_DIR_OUT); //AL422B_FIFO
	gpio0->setDirection(BIT_PWDN,GPIO_DIR_OUT);

	gpio2 = new GpioIntf(GPIO2_BASE);
	gpio2->setDirection(BIT_RRST,GPIO_DIR_OUT); //AL422B_FIFO
	gpio2->setDirection(BIT_WRST,GPIO_DIR_OUT); //AL422B_FIFO
	gpio2->setDirection(BIT_STR,GPIO_DIR_IN);
	gpio2->setDirection(BIT_RST,GPIO_DIR_OUT);
	gpio2->setDirection(BIT_D00,GPIO_DIR_IN);
	gpio2->setDirection(BIT_D01,GPIO_DIR_IN);
	gpio2->setDirection(BIT_D02,GPIO_DIR_IN);
	gpio2->setDirection(BIT_D03,GPIO_DIR_IN);
	gpio2->setDirection(BIT_D04,GPIO_DIR_IN);
	gpio2->setDirection(BIT_D05,GPIO_DIR_IN);
	gpio2->setDirection(BIT_D06,GPIO_DIR_IN);
	gpio2->setDirection(BIT_D07,GPIO_DIR_IN);
	gpio2->setDirection(BIT_HREF,GPIO_DIR_IN);
	gpio2->setDirection(BIT_VSYNC,GPIO_DIR_IN);
}

void Ov7670::read_stop() {
	getData();
	READ_CLOCK_HIGH;
	READ_CLOCK_LOW;
}

void Ov7670::capture_image() {
  capture_next();
  while(captured() == false);
  usleep(10);

  bufferPos = 0;
  if (readImageStartFunctionPtr) {
  	(*readImageStartFunctionPtr)();
  }

  READ_RESET;

  int r=0, g=0, b=0, d1 = 0, d2 = 0, d3 = 0, d4 = 0;
  uint16_t index = 0;

  // read image
  for (int y = 0; y < SIZEY; y++) {
  	for (int x = 0; x < SIZEX; x++) {

  		if (camera_mode != MODE_YUV) {
    		d1 = getData();
    		d2 = getData();
  		}

  		switch (camera_mode) {
  			case MODE_RGB444:
  	  		b = (d1 & 0x0F) << 4;
  				g = (d2 & 0xF0);
  				r = (d2 & 0x0F) << 4;
  				break;
  			case MODE_RGB555:
  	      b = (d1 & 0x1F) << 3;
  	      g = (((d1 & 0xE0) >> 2) | ((d2 & 0x03) << 6));
  	      r = (d2 & 0x7c) << 1;
  				break;
  			case MODE_RGB565:
  	      b = (d1 & 0x1F) << 3;
  	      g = (((d1 & 0xE0) >> 3) | ((d2 & 0x07) << 5));
  	      r = (d2 & 0xF8);
  				break;
  			case MODE_YUV:
  				if (index % 2 == 0) {
  	    		d1 = getData(); // U0
  	    		d2 = getData(); // Y0
    	  		d3 = getData(); // V0
    	  		d4 = getData(); // Y1

            // b = d2 + 1.77200 * (d1 - 128);
            // g = d2 - 0.34414 * (d1 - 128) - 0.71414 * (d3 - 128);
            // r = d2 + 1.40200 * (d3 - 128);

            b = d2 + 1.4075 * (d1 - 128);
            g = d2 - 0.3455 * (d1 - 128) - 0.7169 * (d3 - 128);
            r = d2 + 1.7790 * (d3 - 128);
  				} else {
            // b = d4 + 1.77200 * (d1 - 128);
            // g = d4 - 0.34414 * (d1 - 128) - 0.71414 * (d3 - 128);
            // r = d4 + 1.40200 * (d3 - 128);

            b = d4 + 1.4075 * (d1 - 128);
            g = d4 - 0.3455 * (d1 - 128) - 0.7169 * (d3 - 128);
            r = d4 + 1.7790 * (d3 - 128);
  				}

          b = min(max(b, 0), 255);
          g = min(max(g, 0), 255);
          r = min(max(r, 0), 255);

          index++;
  				break;
  		}

  		buffer[bufferPos] = r;
  		buffer[bufferPos + 1] = g;
  		buffer[bufferPos + 2] = b;
  		bufferPos += 3;
  		if (bufferPos >= BUFFER_SIZE) {
  			if (bufferFullFunctionPtr) {
  				(*bufferFullFunctionPtr)(buffer);
  			}

  			bufferPos = 0;
  		}
  	}
  }

  read_stop();

  if (readImageStopFunctionPtr) {
  	(*readImageStopFunctionPtr)();
  }

}


/**
 * transfers a regval list via SCCB to camera
 *
 * 1: success
 * other values: failure
 */
uint8_t Ov7670::transfer_regvals(struct regval_list *list) {
	uint8_t ret = 0;
	uint8_t i = 0;

	for(;;) {
		// end marker check
		if ((list[i].reg_num == EM) && (list[i].value == EM)) {
			return 1;
		}

		ret = camI2CIntf->writeData(list[i].reg_num, list[i].value);
		if (!ret) {
			return i;
		}

		// delay for reset command
		if ((list[i].reg_num == REG_COM7) && (list[i].value == COM7_RESET)) {
			usleep(1000*200);
		}

		i++;
	}

	return 0;
}


uint8_t Ov7670::init_rgb444_qqvga() {
	uint8_t ret = 0;

	camI2CIntf->writeData(REG_COM7, COM7_RGB | COM7_QQVGA);
	ret = transfer_regvals(ov7670_fmt_rgb444);
	if (ret != 1) return ret;

	return transfer_regvals(ov7670_qqvga);
}

uint8_t Ov7670::init_rgb555_qqvga() {
	uint8_t ret = 0;

	camI2CIntf->writeData(REG_COM7, COM7_RGB | COM7_QQVGA);
	ret = transfer_regvals(ov7670_fmt_rgb555);
	if (ret != 1) return ret;

	return transfer_regvals(ov7670_qqvga);
}

uint8_t Ov7670::init_rgb565_qqvga() {
	uint8_t ret = 0;

	camI2CIntf->writeData(REG_COM7, COM7_RGB | COM7_QQVGA);
	ret = transfer_regvals(ov7670_fmt_rgb565);
	if (ret != 1) return ret;

	return transfer_regvals(ov7670_qqvga);
}

uint8_t Ov7670::init_yuv_qqvga() {
	uint8_t ret = 0;

	camI2CIntf->writeData(REG_COM7, COM7_YUV);
	ret = transfer_regvals(ov7670_fmt_yuv422);
	if (ret != 1) return ret;

	return transfer_regvals(ov7670_qqvga);
}


void Ov7670::init_negative_vsync() {
	camI2CIntf->writeData(REG_COM10, COM10_VS_NEG);
}

void Ov7670::init_camera_reset() {
	camI2CIntf->writeData(REG_COM7, COM7_RESET);
	usleep(1000*200);
}

uint8_t Ov7670::init_default_values() {
	return transfer_regvals(ov7670_default);
}
