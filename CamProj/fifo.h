/*
 * fifo.h
 *
 * Defines for AL422 fifo
 *
 *  Created on: Aug 18, 2013
 *      Author: arndtjenssen
 *		Modified by: daametsi
 */

#ifndef FIFO_H_
#define FIFO_H_

#include "GPIO_CONST.h"

#define WRITE_ENABLE gpio0->setBit(BIT_WE, 1)
#define WRITE_DISABLE gpio0->setBit(BIT_WE, 0)

#define READ_CLOCK_HIGH gpio0->setBit(BIT_RCK, 1)
#define READ_CLOCK_LOW gpio0->setBit(BIT_RCK, 0)

#define READ_RESET_HIGH gpio2->setBit(BIT_RRST, 1)
#define READ_RESET_LOW gpio2->setBit(BIT_RRST, 0)

#define WRITE_RESET_HIGH gpio2->setBit(BIT_WRST, 1)
#define WRITE_RESET_LOW gpio2->setBit(BIT_WRST, 0)

#define READ_RESET READ_RESET_LOW; READ_CLOCK_HIGH; READ_RESET_HIGH; READ_CLOCK_LOW;

#define WRITE_RESET WRITE_RESET_LOW; WRITE_RESET_HIGH;

#endif /* FIFO_H_ */
