/*
 * fram.h
 *
 *  Created on: Apr 6, 2023
 *      Author: mdaef
 */

#ifndef INC_FRAM_H_
#define INC_FRAM_H_

#include "stm32f1xx_hal.h"


void FRAM_ID(void);
void FRAM_enablewrite(void);
void FRAM_resetWrite(void);
void FRAM_StatusRegister(void);
void FRAM_Write(uint16_t address, uint8_t *data, uint16_t size);
void FRAM_Read(uint16_t address, uint8_t *data_receive, uint16_t size);
void SendData_to_FRAM(float *memory_x, float *memory_y, float *memory_z, int size);

#endif /* INC_FRAM_H_ */
