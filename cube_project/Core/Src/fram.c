/*
 * fram.c
 *
 *  Created on: Apr 6, 2023
 *      Author: mdaef
 */

#include "fram.h"

extern SPI_HandleTypeDef hspi1;
FRAM_STATES FRAM_state;

// Commands
uint8_t WRSR  = 0x01;
uint8_t WRITE = 0x02;
uint8_t READ  = 0x03;
uint8_t WRDI  = 0x04;
uint8_t RDSR  = 0x05;
uint8_t WREN  = 0x06;
uint8_t RDIDI  = 0x9F;
//uint8_t SLEEP = 0xB9;

uint8_t bytes_ID[4];
uint8_t buffer_ID[1]= {0x9F};

uint8_t byte[1]= {0};
uint8_t msg[128];

uint8_t receiv_Data[16];
uint8_t send_Data[16];
float transf_float[4];
uint32_t DATA_FRAM[4];
uint8_t i = 0;

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi1) {
        if (hspi->Instance->CR1 & SPI_CR1_SPE) {
 	  	  switch(FRAM_state)
 	  	{
 	  		case WAIT_ENABLE_WRITE:
 	  			FRAM_state = FRAM_WRITE;
 	  			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //OK
 	  		break;
 	  		case WAIT_FRAM_WRITE:
 	  			FRAM_state = FRAM_READ_COMMAND;
 	  			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //OK
 	  		break;
 	  		case WAIT_FRAM_READ_COMMAND:
 	  			FRAM_state = FRAM_READ;
 	  		break;
 	  		default:
 	  			break;
 	  	}
      }
   }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi1) {
        if (hspi->Instance->CR1 & SPI_CR1_SPE) {
 	  	  switch(FRAM_state)
 	  	{
 	  		case WAIT_FRAM_READ_ID:
 	  			FRAM_state = ENABLE_WRITE;
 	  			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //OK
 	  		break;
 	  		case WAIT_FRAM_READ:
 	  			FRAM_state = FRAM_IDLE;
 	  			DATA_FRAM[0] = (receiv_Data[3] << 24) | (receiv_Data[2] << 16) | (receiv_Data[1] << 8) | receiv_Data[0];
	  		    transf_float[0] = *(float*)&DATA_FRAM[0];
	  		    send_Data [0] = receiv_Data[3];
	  		    send_Data [1] = receiv_Data[2];
	  		    send_Data [2] =  receiv_Data[1];
	  		    send_Data [3] = receiv_Data[0];

	  		    DATA_FRAM[1] = (receiv_Data[7] << 24) | (receiv_Data[6] << 16) | (receiv_Data[5] << 8) | receiv_Data[4];
	  		    transf_float[1] = *(float*)&DATA_FRAM[1];
	  		    send_Data [4] = receiv_Data[7];
	  		    send_Data [5] = receiv_Data[6];
	  		    send_Data [6] =  receiv_Data[5];
	  		    send_Data [7] = receiv_Data[4];

	  		    DATA_FRAM[2] = (receiv_Data[11] << 24) | (receiv_Data[10] << 16) | (receiv_Data[9] << 8) | receiv_Data[8];
	  		    transf_float[2] = *(float*)&DATA_FRAM[2];
	  		    send_Data [8] = receiv_Data[11];
	  		    send_Data [9] = receiv_Data[10];
	  		    send_Data [10] =  receiv_Data[9];
	  		    send_Data [11] = receiv_Data[8];

	  		    DATA_FRAM[3] = (receiv_Data[15] << 24) | (receiv_Data[14] << 16) | (receiv_Data[13] << 8) | receiv_Data[12];
	  		    transf_float[3] = *(float*)&DATA_FRAM[3];
	  		    send_Data [12] = receiv_Data[15];
	  		    send_Data [13] = receiv_Data[14];
	  		    send_Data [14] =  receiv_Data[13];
	  		    send_Data [15] = receiv_Data[12];

 	  			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //OK
 	  			byte[0] = 0x02;
 	  		break;
 	  		default:
 	  			break;
 	  	}
      }
   }
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
    byte[0] = 0x03;
}

void FRAM_ID(void) {

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_SPI_Transmit_IT(&hspi1, buffer_ID, 1);
    HAL_SPI_Receive_IT(&hspi1, bytes_ID, 4);

}


void FRAM_enablewrite(void){

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

    HAL_SPI_Transmit_IT(&hspi1, (uint8_t *)&WREN, 1);

}

void FRAM_StatusRegister(void){

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_SPI_Transmit_IT(&hspi1, (uint8_t *)&RDSR, 1);
}

void FRAM_resetWrite(void){

	uint8_t addr_high = 0x00;
    uint8_t addr_low = 0x00;

    msg[0] = WRDI;
    msg[1] = addr_high;
    msg[2] = addr_low;

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

    HAL_SPI_Transmit_IT(&hspi1, msg, 3);
}

void FRAM_Write(uint16_t address, uint8_t *data, uint16_t size){

	uint8_t addr_high = (address >> 8 ) & 0xFF;
    uint8_t addr_low = address & 0xFF;

    msg[0] = WRITE;
    msg[1] = addr_high;
    msg[2] = addr_low;

    for (int i = 0; i < size ; i++){
    	msg[i+3] = data[i];
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_SPI_Transmit_IT(&hspi1, msg, 3+size);

//  HAL_SPI_Transmit_IT(&hspi1, data, size);

}

void FRAM_Read_Command(uint16_t address){

	uint8_t addr_high = (address >> 8 ) & 0xFF;
    uint8_t addr_low = address & 0xFF;

    msg[0] = READ;
    msg[1] = addr_high;
    msg[2] = addr_low;

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_SPI_Transmit_IT(&hspi1, msg, 3);
}

void FRAM_Read(uint8_t *data_receive, uint16_t size){

    //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

    HAL_SPI_Receive_IT(&hspi1, data_receive, size);
}
