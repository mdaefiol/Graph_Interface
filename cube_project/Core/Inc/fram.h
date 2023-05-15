/*
 * fram.h
 *
 *  Created on: Apr 6, 2023
 *      Author: mdaef
 */

#ifndef INC_FRAM_H_
#define INC_FRAM_H_

#include "stm32f1xx_hal.h"

typedef enum {
	FRAM_IDLE,
	FRAM_READ_ID,
	WAIT_FRAM_READ_ID,
	WAIT_FRAM_READ_ID2,
	WAIT_FRAM_READ_ID3,
    ENABLE_WRITE,
    WAIT_ENABLE_WRITE,
	SEND_DATA,
    FRAM_WRITE,
    WAIT_FRAM_WRITE,
	FRAM_READ_COMMAND,
	WAIT_FRAM_READ_COMMAND,
    FRAM_READ,
    WAIT_FRAM_READ
} FRAM_STATES;

void FRAM_ID(void);
void FRAM_enablewrite(void);
void FRAM_resetWrite(void);
void FRAM_StatusRegister(void);
void FRAM_Write(uint16_t address, uint8_t *data, uint16_t size);
void FRAM_Read(uint8_t *data_receive, uint16_t size);
void FRAM_Read_Command(uint16_t address);
void SendData_to_FRAM(float *memory_x, float *memory_y, float *memory_z, int size);

#endif /* INC_FRAM_H_ */
