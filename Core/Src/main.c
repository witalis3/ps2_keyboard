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
  * OLED, klawiatura, keypad
  * --------------------------
  * SD na SPI3
  * CS PA0 biały (przy module) -> zielony (analizator)
  * MOSI PB2 niebieski fioletowy (analizator)
  * MISO PC11 czarny -> niebieski (analizator)
  * SCK PC10 żółty	-> żółty (analizator)
  *
  *
  *
  * ToDo bieżące
  *
  * wyłuskanie nazwy pliku i katalogu z dwóch struktur: FILINFO I FATFS
  *
  *
  * 	- nowy branch z przykładami wykorzystania biblioteki Chana ver. 0.15 (Chan_FatFS_015) -> implementacja
  * 	- analiza na analizatorze braku inicjalizacji -> na potem
  * 		- CLK i MOSI nie startują po inicjalizacji; może zmiana zegara ma wpływ?
  * 			- niby po delay pojawił się dodakowy dołek/impuls w CS po ini
  *
  *------------------------------------------------------
  * 	- pamiętać:
  * 		- u nas jest:
  * 			- hspi3 (hspi trzy)
  * 			- huart2
  * 				- PA2 TXD
  * 				- PA3 RXD
  *
  * ToDo na później
  * 	- obsługa dwóch kart
  * 	- identyfikacja klawiatury (model/producent) w sytemie na PC
  * 	- keypad ewentualnie na przerwaniach: są opóźnienia na klawiaturze spowodowane
  * 	debouncingiem i timeoutem w obsłudze KeyPada
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sd.h"

#include "keyboard.h"
#include "queue.h"
#include <string.h>
#include <stdio.h>
#include "ssd1306.h"
#include "KeyPad.h"
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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint16_t Timer1=0;
//uint8_t sect[512];
//char buffer1[512] ="Selection ... The..."; //bufor danych dla zapisu i odczytu
uint8_t sect[512];
// z urok 3:
extern char str1[60];
uint32_t byteswritten,bytesread;
uint8_t result;
extern char USERPath[4]; /* logical drive path */
FATFS SDFatFs;
FATFS *fs;
FIL MyFile;
FRESULT fr;
FILINFO fno;

//---------------------
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
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
FRESULT ReadLongFile(void)

{
  uint16_t i=0, i1=0;
  uint32_t ind=0;
  uint32_t f_size = fno.fsize;
	sprintf(str1, "fsize: %lu\r\n", (unsigned long) f_size);
	HAL_UART_Transmit(&huart2, (uint8_t*) str1, strlen(str1), 0x1000);
	ind = 0;
	do
	{
		if (f_size < 512)
		{
			i1 = f_size;
		}
		else
		{
			i1 = 512;
		}
		f_size -= i1;
		f_lseek(&MyFile, ind);
		f_read(&MyFile, sect, i1, (UINT*) &bytesread);
		for (i = 0; i < bytesread; i++)
		{
			HAL_UART_Transmit(&huart2, sect + i, 1, 0x1000);
		}
		ind += i1;
	}
	while (f_size > 0);
	HAL_UART_Transmit(&huart2, (uint8_t*) "\r\n", 2, 0x1000);
	return FR_OK;
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
	uint16_t i;
	FRESULT res; /* FatFs function common result code */
	uint8_t wtext[] = "STM32 FATFS works jako tako"; /* File write buffer */
	FILINFO fileInfo;
	char *fn;
	DIR dir;
	DWORD fre_clust, fre_sect, tot_sect;

	uint32_t byteswritten; /* File write/read counts */
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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_USB_DEVICE_Init();
  MX_FATFS_Init();
  MX_TIM2_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  //HAL_Delay(1000);

  HAL_TIM_Base_Start_IT(&htim2);
  //SD_PowerOn();
  //sd_ini();

  // zapis i odczyt z pominięciem systemu plików:
  //SD_Write_Block((uint8_t*)buffer1,0x0400); //zapisujemy blok w bufor na konkretny adres
  //SD_Read_Block(sect,0x0400); //wczytujemy blok z bufora (karty)
  //for(i=0;i<512;i++) HAL_UART_Transmit(&huart2, sect+i, 1, 0x1000);
  //HAL_UART_Transmit(&huart2,(uint8_t*)"\r\n",2,0x1000);

  disk_initialize(SDFatFs.drv);
  /*
  //read
  	if (f_mount(&SDFatFs, (TCHAR const*) USERPath, 0) != FR_OK)
	{
		Error_Handler();
	}
	else
	{
		fr = f_stat("123.txt", &fno);
		if (f_open(&MyFile, "123.txt", FA_READ) != FR_OK)
		{
			Error_Handler();
		}
		else
		{
			ReadLongFile();
			f_close(&MyFile);
		}
	}
*/
/*
  //write

  if(f_mount(&SDFatFs,(TCHAR const*)USERPath,0)!=FR_OK)
  {
    Error_Handler();
  }
  else
  {
    if(f_open(&MyFile,"mywrite.txt",FA_CREATE_ALWAYS|FA_WRITE)!=FR_OK)
    {
      Error_Handler();
    }

    else
    {
      res=f_write(&MyFile,wtext,sizeof(wtext),(void*)&byteswritten);
      if((byteswritten==0)||(res!=FR_OK))
      {
        Error_Handler();
      }
      f_close(&MyFile);
    }
  }
*/
//read dir
  if(f_mount(&SDFatFs,(TCHAR const*)USERPath,0)!=FR_OK)
  {
    Error_Handler();
  }
  else
  {
	SDFatFs.lfnbuf = (char*)sect;
    fileInfo.fsize = sizeof(sect);
    result = f_opendir(&dir, "/");
    if (result == FR_OK)
    {
    	while(1)
    	  {
    	    result = f_readdir(&dir, &fileInfo);
    	    if (result==FR_OK && fileInfo.fname[0])
    	    {
    	    	//fn = fileInfo.lfname; ???
    	    	fn = SDFatFs.lfnbuf;
    	    	  if(strlen(fn))
    	    	  {
    	    		  HAL_UART_Transmit(&huart2,(uint8_t*) fn, strlen(fn),0x1000);
    	    	  }
    	    	  else
    	    		  {
    	    		  	  HAL_UART_Transmit(&huart2,(uint8_t*)fileInfo.fname, strlen((char*)fileInfo.fname),0x1000);
    	    		  }
    	    	  if(fileInfo.fattrib & AM_DIR)
    	    	  {
    	    	    HAL_UART_Transmit(&huart2,(uint8_t*)" [DIR]",7,0x1000);
    	    	  }
    	    }
    	    else break;
    	    HAL_UART_Transmit(&huart2,(uint8_t*)"\r\n",2,0x1000);
    	  }
      f_closedir(&dir);
    }
  }
  FATFS_UnLinkDriver(USERPath);



  queue_init(&keyq);
  const char message[] = "Keyboard started!\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  	// inicjalizacja OLED
  	ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0,26);
    ssd1306_WriteString("Modulo 3 ele", Font_11x18, White);
    ssd1306_UpdateScreen();

  while (1)
  {
	  // ToDo opóźnienie i ini później do usunięcia
	  HAL_Delay(100);
	  handle_keys(&hUsbDeviceFS, &khid, &keyq, keyq_timeout, &hi2c1);
	#ifdef DEBUG
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
	  	  //HAL_GPIO_TogglePin(SD_CS_GPIO_Port, SD_CS_Pin);
	  	  //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

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
			 */
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
  RCC_OscInitStruct.PLL.PLLN = 32;
  RCC_OscInitStruct.PLL.PLLP = 10;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
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
