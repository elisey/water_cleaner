#pragma once

#include <stdint.h>
#include "gpio.h"
#include <stdbool.h>


typedef enum	{
	pinPullState_Floating,
	pinPullState_PullUp
} pinPullState_t;

typedef struct
{
	GPIO_TypeDef *gpio;
	GPIO_Pin_TypeDef pin;
} pinInput_t;

void PinInput_Init(pinInput_t *this, GPIO_TypeDef *gpio, GPIO_Pin_TypeDef pin, pinPullState_t pullState);
bool PinInput_Read(pinInput_t *this);