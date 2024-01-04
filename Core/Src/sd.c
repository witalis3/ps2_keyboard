/*
 * sd.c
 *
 *  Created on: Aug 27, 2023
 *      Author: witol
 */

#include "sd.h"
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi3;

//-----------------------------------------------

static void Error (void)

{

	myprintf("\r\n~ blad SD red ~\r\n\r\n");

}
//-----------------------------------------------

//-----------------------------------------------

uint8_t SPIx_WriteRead(uint8_t Byte)

{

  uint8_t receivedbyte = 0;

  if(HAL_SPI_TransmitReceive(&hspi1,(uint8_t*) &Byte,(uint8_t*) &receivedbyte,1,0x1000)!=HAL_OK)
  {
    Error();
  }
  if(HAL_SPI_TransmitReceive(&hspi3,(uint8_t*) &Byte,(uint8_t*) &receivedbyte,1,0x1000)!=HAL_OK)
  {
    Error();
  }
  return receivedbyte;

}

//-----------------------------------------------



//Poniżej tej funkcji dodamy także trzy funkcje dla SPI - odczyt, zapis i normalny przebieg bajtów



//-----------------------------------------------

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

//-----------------------





  uint8_t receivedbyte = 0;

  extern volatile uint16_t Timer1;
  sd_info_ptr sdinfo;

  uint8_t sd_ini(void)

  {

    uint8_t i;

    int16_t tmr;

    uint32_t temp;

    //LD_OFF;

    sdinfo.type = 0;



  //Zapiszmy wartość dzielnika magistrali SPI, ustawmy na razie inny dzielnik i zainicjujmy ponownie magistralę, aby zmniejszyć prędkość, ponieważ karta może nie od razu rozumieć zbyt szybko



  //sdinfo.type = 0;

  temp = hspi1.Init.BaudRatePrescaler;

  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; //156.25 kbbs

  HAL_SPI_Init(&hspi1);

  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; //156.25 kbbs

    HAL_SPI_Init(&hspi3);


  //Następnie wyślemy kilka impulsów na pin zegara SPI. Według arkusza danych potrzebujesz co najmniej 74, na wszelki wypadek wyślemy 80. Można to zrobić po prostu przepuszczając bajt przez magistralę SPI. Jest jeszcze jeden wymóg - noga selekcyjna musi zostać podniesiona



  //HAL_SPI_Init(&hspi1);

  SS_SD_DESELECT();

  for(i=0;i<10;i++) //80 импульсов (не менее 74) Даташит стр 91

    SPI_Release();
  }


//-----------------------------------------------



//Poniżej tej funkcji dodamy także trzy funkcje dla SPI - odczyt, zapis i normalny przebieg bajtów



//-----------------------------------------------


//-----------------------------------------------


//-----------------------------------------------


//-----------------------------------------------
//-----------------------------------------------


//-----------------------------------------------

extern volatile uint16_t Timer1;

// Stwórzmy funkcję włączającą, w której po prostu czekamy 20 milisekund, aż napięcie się ustabilizuje.

//-----------------------------------------------

void SD_PowerOn(void)

{
  Timer1 = 0;

  while(Timer1<2) //ждём 20 милисекунд, для того, чтобы напряжение стабилизировалось

    ;

}

//-----------------------------------------------



// Również poniżej stworzymy w nim funkcję inicjalizacji karty SD



//-----------------------------------------------


//-----------------------------------------------
//--------------------------------------------------


