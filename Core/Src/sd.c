/*
 * sd.c
 *
 *  Created on: Sep 10, 2023
 *      Author: witol
 */
#include "sd.h"
extern volatile uint16_t Timer1;
sd_info_ptr sdinfo;
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;

void SD_PowerOn(void)
{
	Timer1 = 0;
	while (Timer1 < 2)
		//ждём 20 милисекунд, для того, чтобы напряжение стабилизировалось
		;
}

//-----------------------------------------------
uint8_t sd_ini(void)
{
	uint8_t i;

	int16_t tmr;

	uint32_t temp;

	LD_OFF
	;

	sdinfo.type = 0;
	temp = hspi1.Init.BaudRatePrescaler;

	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; //156.25 kbbs

	HAL_SPI_Init(&hspi1);
	SS_SD_DESELECT();

	for (i = 0; i < 10; i++)
		//80 импульсов (не менее 74) Даташит стр 91
	{
		SPI_Release();
	}
	return 0;
}
static void Error(void)
{

	LD_ON;

}
uint8_t SPIx_WriteRead(uint8_t Byte)
{

	uint8_t receivedbyte = 0;

	if (HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) &Byte,
			(uint8_t*) &receivedbyte, 1, 0x1000) != HAL_OK)

	{

		Error();

	}

	return receivedbyte;

}
void SPI_SendByte(uint8_t bt)
{

	SPIx_WriteRead(bt);

}

//-----------------------------------------------

uint8_t SPI_ReceiveByte(void)

{

	uint8_t bt = SPIx_WriteRead(0xFF);

	return bt;

}

//-----------------------------------------------

void SPI_Release(void)

{

	SPIx_WriteRead(0xFF);

}
