/*
 * fram.c
 *
 *  Created on: Apr 6, 2023
 *      Author: mdaef
 */

#include "fram.h"

extern SPI_HandleTypeDef hspi1;
extern enum FRAM_STATES state;
// Commands
uint8_t WRSR  = 0x01;
uint8_t WRITE = 0x02;
uint8_t READ  = 0x03;
uint8_t WRDI  = 0x04;
uint8_t RDSR  = 0x05;
uint8_t WREN  = 0x06;
uint8_t RDIDI  = 0x9F;
//uint8_t SLEEP = 0xB9;

char bytes_ID[4] = {0};
char bytes_status = {0};

uint8_t byte[1]= {0};
uint8_t msg[128];

uint8_t spiCommandPending = 1;

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi1) {
        if (hspi->Instance->CR1 & SPI_CR1_SPE) {
            if (spiCommandPending == 1) {
                spiCommandPending = 0;
            } else {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            }
            byte[0] = 0x02;
        }
    }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi1) {
        if (hspi->Instance->CR1 & SPI_CR1_SPE) {
            if (spiCommandPending == 1) {
                spiCommandPending = 0;
            } else {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            }
        }
    }
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
    if (spiCommandPending == 1) {
        spiCommandPending = 0;
    }
    byte[0] = 0x03;
}

void FRAM_ID(void) {

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_SPI_Transmit_IT(&hspi1, (uint8_t *)&RDIDI, 1);
    HAL_SPI_Receive_IT(&hspi1, (uint8_t *)bytes_ID, 4);
    spiCommandPending = 1;
}


void FRAM_enablewrite(void){

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

    HAL_SPI_Transmit_IT(&hspi1, (uint8_t *)&WREN, 1);
    spiCommandPending = 1;
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

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_SPI_Transmit_IT(&hspi1, msg, 3);
    spiCommandPending = 0;
    HAL_SPI_Transmit_IT(&hspi1, data, size);
    spiCommandPending = 1;
}

void FRAM_Read(uint16_t address, uint8_t *data_receive, uint16_t size){
    uint8_t addr_high = (address >> 8 ) & 0xFF;
    uint8_t addr_low = address & 0xFF;

    msg[0] = READ;
    msg[1] = addr_high;
    msg[2] = addr_low;

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_SPI_Transmit_IT(&hspi1, msg, 3);

    HAL_SPI_Receive_IT(&hspi1, data_receive, size);
    spiCommandPending = 1;
}



