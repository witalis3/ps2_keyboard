#ifndef	_KEYPADCONFIG_H
#define	_KEYPADCONFIG_H
#include "gpio.h"

#define           _KEYPAD_DEBOUNCE_TIME_MS        10
#define           _KEYPAD_USE_FREERTOS            0

const GPIO_TypeDef* _KEYPAD_COLUMN_GPIO_PORT[] =
{
  GPIOE,
  GPIOE,
  GPIOE,
  GPIOE
};

const uint16_t _KEYPAD_COLUMN_GPIO_PIN[] =
{
  GPIO_PIN_13,
  GPIO_PIN_11,
  GPIO_PIN_9,
  GPIO_PIN_7
};

const GPIO_TypeDef* _KEYPAD_ROW_GPIO_PORT[] =
{
  GPIOB,
  GPIOC,
  GPIOA,
  GPIOA
};

const uint16_t _KEYPAD_ROW_GPIO_PIN[] =
{
  GPIO_PIN_1,
  GPIO_PIN_5,
  GPIO_PIN_7,
  GPIO_PIN_5
};

#endif
