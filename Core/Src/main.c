/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  *
  * działa SD, OLED, klawiatura, keypad
  *
  * do zapamiętania: po generacji z Cube MX zmiania się zawartość pliku usbd_customhid.c
  * powinno być:
  * 0x01,                                          bInterfaceSubClass : 1=BOOT, 0=no boot
  * 0x01,                                              nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
  *
  * ToDo
  * 	- lekcja 1
  * 	- obsługa dwóch kart
  * 	- identyfikacja klawiatury (model/producent) w sytemie na PC
  * 	- keypad ewentualnie na przerwaniach: są opóźnienia na klawiaturze spowodowane
  * 	debouncingiem i timeoutem w obsłudze KeyPada
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// SD SPI
#include "sd.h"

#include "keyboard.h"
#include "queue.h"
#include <string.h>
#include <stdio.h>
#include "ssd1306.h"
#include "KeyPad.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions
//#include <ssd1306_tests.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SSD1306_USE_I2C
#define __DEBUG 1
#define BUFFERSIZE 100
#define I2CBUF	12
#define debug_print(x) 	do { if ( __DEBUG ) { strcpy(uartBuffer, x); HAL_UART_Transmit(&huart2, (unsigned char*) uartBuffer, strlen(uartBuffer), HAL_MAX_DELAY); }} while (0)
FATFS fs;
FIL fil;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// SD SPI
volatile uint16_t Timer1=0;

struct queue_t keyq = {0};
struct keyboard_hid_t khid = {0};
uint32_t keyq_timeout = 0;
// dla OLED:
const uint8_t SSD1306_ADDRESS = 0x3C << 1;
const uint8_t RANDOM_REG = 0x0F;

char uartBuffer[BUFFERSIZE] = "";
uint8_t I2CBuffer[I2CBUF] = {0};
HAL_StatusTypeDef returnValue = 0;
// koniec OLED

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void myprintf(const char *fmt, ...);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void myprintf(const char *fmt, ...)
{
	  static char buffer[256];
	  va_list args;
	  va_start(args, fmt);
	  vsnprintf(buffer, sizeof(buffer), fmt, args);
	  va_end(args);

	  int len = strlen(buffer);
	 // HAL_UART_Transmit(&huart2, (uint8_t*)buffer, len, HAL_MAX_DELAY);
}
void init()
{
	//ssd1306_TestAll();
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */


	FRESULT res; /* FatFs function common result code */
	uint32_t byteswritten; /* File write/read counts */
	uint8_t wtext[] = "STM32 FATFS works jako tako"; /* File write buffer */
	//uint8_t rtext[_MAX_SS];/* File read buffer */

	KeyPad_Init();

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FATFS_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  // SD SPI:
  HAL_TIM_Base_Start_IT(&htim2);


  //queue_init(&keyq);
  //const char message[] = "Keyboard started!\r\n";
  //HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
  /* USER CODE BEGIN 2 */
  //const char message[] = "SD card demo by kiwih\r\n";
  //HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
 // myprintf("\r\n~ SD card demo by kiwih ~\r\n\r\n");
  // SD SPI
      HAL_TIM_Base_Start_IT(&htim2);

      SD_PowerOn();

      sd_ini();


  //HAL_Delay(1000); //a short delay is important to let the SD card settle

  //HAL_Delay(500);
/*
  f_mount(&fs, "", 0);
  f_open(&fil, "write.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
  //f_lseek(&fil, fil.fsize);
  f_puts("Hello from Nizar\n", &fil);
  f_close(&fil);


  //some variables for FatFs
  FATFS FatFs; 	//Fatfs handle
  FIL fil; 		//File handle
  FRESULT fres; //Result after operations

  //Open the file system
  fres = f_mount(&FatFs, "", 1); //1=mount now
  if (fres != FR_OK) {
	myprintf("f_mount error (%i)\r\n", fres);
	while(1);
  }

  //Let's get some statistics from the SD card
  DWORD free_clusters, free_sectors, total_sectors;

  FATFS* getFreeFs;

  fres = f_getfree("", &free_clusters, &getFreeFs);
  if (fres != FR_OK) {
	myprintf("f_getfree error (%i)\r\n", fres);
	while(1);
  }

  //Formula comes from ChaN's documentation
  total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
  free_sectors = free_clusters * getFreeFs->csize;

  myprintf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);

  //Now let's try to open file "test.txt"
  fres = f_open(&fil, "test.txt", FA_READ);
  if (fres != FR_OK) {
	myprintf("f_open error (%i)\r\n");
	while(1);
  }
  myprintf("I was able to open 'test.txt' for reading!\r\n");

  //Read 30 bytes from "test.txt" on the SD card
  BYTE readBuf[30];

  //We can either use f_read OR f_gets to get data out of files
  //f_gets is a wrapper on f_read that does some string formatting for us
  TCHAR* rres = f_gets((TCHAR*)readBuf, 30, &fil);
  if(rres != 0) {
	myprintf("Read string from 'test.txt' contents: %s\r\n", readBuf);
  } else {
	myprintf("f_gets error (%i)\r\n", fres);
  }

  //Be a tidy kiwi - don't forget to close your file!
  f_close(&fil);

  //Now let's try and write a file "write.txt"
  fres = f_open(&fil, "write.txt", FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
  if(fres == FR_OK) {
	myprintf("I was able to open 'write.txt' for writing\r\n");
  } else {
	myprintf("f_open error (%i)\r\n", fres);
  }

  //Copy in a string
  strncpy((char*)readBuf, "a new file is made!", 19);
  UINT bytesWrote;
  fres = f_write(&fil, readBuf, 19, &bytesWrote);
  if(fres == FR_OK) {
	myprintf("Wrote %i bytes to 'write.txt'!\r\n", bytesWrote);
  } else {
	myprintf("f_write error (%i)\r\n");
  }

  //Be a tidy kiwi - don't forget to close your file!
  f_close(&fil);

  //We're done, so de-mount the drive
  f_mount(NULL, "", 0);

*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  	//init();

  	// inicjalizacja OLED
  /*
  	ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0,26);
    ssd1306_WriteString("Modulo 3 ele", Font_11x18, White);
    ssd1306_UpdateScreen();
*/

  while (1)
  {
	  //shandle_keys(&hUsbDeviceFS, &khid, &keyq, keyq_timeout, &hi2c1);
	#ifdef DEBUGi
	  uint8_t bit = HAL_GPIO_ReadPin(CLK_GPIO_Port, CLK_Pin);
	  	  if (bit == 0)
	  	  {
	  		  HAL_GPIO_WritePin(DEBUG1_GPIO_Port, DEBUG1_Pin, GPIO_PIN_RESET);
	  	  }
	  	  else
	  	  {
	  		  HAL_GPIO_WritePin(DEBUG1_GPIO_Port, DEBUG1_Pin, GPIO_PIN_SET);
	  	  }
	  	  //HAL_GPIO_TogglePin(DEBUG1_GPIO_Port, DEBUG1_Pin);
		  bit = HAL_GPIO_ReadPin(DIN_GPIO_Port, DIN_Pin);
	  	  if (bit == 0)
	  	  {
	  		  HAL_GPIO_WritePin(DEBUG2_GPIO_Port, DEBUG2_Pin, GPIO_PIN_RESET);
	  	  }
	  	  else
	  	  {
	  		  HAL_GPIO_WritePin(DEBUG2_GPIO_Port, DEBUG2_Pin, GPIO_PIN_SET);
	  	  }
	  	  /*
	  	  HAL_GPIO_TogglePin(PC8_GPIO_Port, PC8_Pin);
	  	  HAL_GPIO_TogglePin(PC9_GPIO_Port, PC9_Pin);
	  	  HAL_GPIO_TogglePin(PC10_GPIO_Port, PC10_Pin);
	  	  HAL_GPIO_TogglePin(PC11_GPIO_Port, PC11_Pin);
	  	  HAL_GPIO_TogglePin(PC12_GPIO_Port, PC12_Pin);
	  	  HAL_GPIO_TogglePin(PD2_GPIO_Port, PD2_Pin);

	  	  HAL_GPIO_TogglePin(PA5_GPIO_Port, PA5_Pin);
	  	  HAL_GPIO_TogglePin(PA7_GPIO_Port, PA7_Pin);
	  	  HAL_GPIO_TogglePin(PC5_GPIO_Port, PC5_Pin);
	  	  HAL_GPIO_TogglePin(PB1_GPIO_Port, PB1_Pin);

	  	  HAL_GPIO_TogglePin(PE7_GPIO_Port, PE7_Pin);
	  	  HAL_GPIO_TogglePin(PE9_GPIO_Port, PE9_Pin);
	  	  HAL_GPIO_TogglePin(PE11_GPIO_Port, PE11_Pin);
	  	  HAL_GPIO_TogglePin(PE13_GPIO_Port, PE13_Pin);
	  	  // SD2:
	  	  HAL_GPIO_TogglePin(PA0_GPIO_Port, PA0_Pin);
	  	  HAL_GPIO_TogglePin(PB3_GPIO_Port, PB3_Pin);
	  	  HAL_GPIO_TogglePin(PB4_GPIO_Port, PB4_Pin);
	  	  HAL_GPIO_TogglePin(PB14_GPIO_Port, PB14_Pin);
	  	  HAL_GPIO_TogglePin(PB15_GPIO_Port, PB15_Pin);
	  	  HAL_GPIO_TogglePin(PC1_GPIO_Port, PC1_Pin);
	  	  */

	#endif
			 //const char message[] = "petla po zapisie\r\n";
			  //HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
			//HAL_Delay(500);
			 uint16_t keypad;
			 uint32_t timeout = 20;
			 /*
			 keypad = KeyPad_WaitForKey(timeout);
			 if (keypad != 0)
			 {

					 //const char message[] = "blad zapisu\r\n";
					 char message[12];
					 sprintf(message, "%d\r\n", keypad);
					  HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);

			 }

			 char znak = KeyPad_WaitForKeyGetChar(timeout);
			 if (znak != 'X')
			 {
				  	ssd1306_Init();
				    ssd1306_Fill(Black);
				    ssd1306_SetCursor(0,26);
				    //char c = znak;
				    char str1[2] = {znak , '\0'};
					ssd1306_WriteString(str1, Font_11x18, White);
					ssd1306_UpdateScreen();
			 }

*/
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 12;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOMEDIUM;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// SD SPI
//----------------------------------------------------------

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)

{

if(htim==&htim2)

{

Timer1++;

}

}

//----------------------------------------------------------

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
  if (pin == CLK_Pin)
  {
    ps2_clk_callback(&keyq, &keyq_timeout, DIN_GPIO_Port, DIN_Pin);
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {

  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
