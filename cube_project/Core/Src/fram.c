/*
 * fram.c
 *
 *  Created on: Apr 6, 2023
 *      Author: mdaef
 */

#include "fram.h"

extern SPI_HandleTypeDef hspi1;

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
char bytes_status[3] = {0} ;


void FRAM_ID(void){

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi1, (uint8_t *)&RDIDI, 1, 100); //envia
	HAL_SPI_Receive(&hspi1,  (uint8_t *)bytes_ID, 4, 100); //recebe dado

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
}

void FRAM_enablewrite(void){

	//Ativa trava de habilitação de gravação (permite operações de gravação)
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi1, (uint8_t *)&WREN, 1, 100);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

}

void FRAM_resetWrite(void){

	uint8_t addr_high = 0x00;
	uint8_t addr_low = 0x00;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi1, (uint8_t *)&WRDI, 1, 100);
	HAL_SPI_Transmit(&hspi1, &addr_high, 1, 100);
	HAL_SPI_Transmit(&hspi1, &addr_low, 1, 100);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

}

void FRAM_StatusRegister(void){

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi1, (uint8_t *)&RDSR, 1, 100);
	HAL_SPI_Receive(&hspi1, (uint8_t *)bytes_status, 1, 100); //recebe dado

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

}

void FRAM_Write(uint16_t address, uint8_t *data, uint16_t size){

	// write
	uint8_t addr_high = (address >> 8 ) & 0xFF;
	uint8_t addr_low = address & 0xFF;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi1, (uint8_t *)&WRITE, 1, 100);
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&addr_high, 1, 100);
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&addr_low, 1, 100);
	HAL_SPI_Transmit(&hspi1, data, size, 100);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

}

void FRAM_Read(uint16_t address, uint8_t *data_receive, uint16_t size){

	// read
	uint8_t addr_high = (address >> 8 ) & 0xFF;
	uint8_t addr_low = address & 0xFF;

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi1, (uint8_t *)&READ, 1, 100);
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&addr_high, 1, 100);
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&addr_low, 1, 100);
	HAL_SPI_Receive(&hspi1, data_receive, size, 100); //recebe dado

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
}


/*
void SendData_to_FRAM(float *memory_x, float *memory_y, float *memory_z, int size) {

	// Escreve os dados na FRAM
    for (int i = 0; i < size; i++) {
        // Seleciona o dispositivo
    	FRAM_enablewrite();

        // Escreve os dados de aceleração nos três eixos
        uint8_t data[12];
        memcpy(data, &memory_x[i], 4);
        memcpy(data + 4, &memory_y[i], 4);
        memcpy(data + 8, &memory_z[i], 4);
        FRAM_Write(0x6000, data, 12);
    }
}

uint8_t spi_tx_rx(uint8_t tx){ //1byte
	uint8_t rx;
	uint8_t mask = 0x01;

	for(int i=7; i>=0; i--)
	{
		if (tx & (mask << i) )
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		}
		HAL_Delay(1);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_Delay(1);

	}
	return rx;
}


void FRAM_MaskOut(void){

	// Espera até que o bit WIP seja apagado
	  wip = 1;

	  while (wip)
	  {
	    // Lê o registro de status
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	    HAL_SPI_Transmit(&hspi1, (uint8_t *)&RDSR, 1, 100);
	    HAL_SPI_Receive(&hspi1, (uint8_t *)spi_buf, 1, 100);
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	    // Mask out WIP bit
	    wip = spi_buf[0] & 0b00000001;
	  }
}
*/

