#pragma once

#include <stdint.h>
#include "gpio.h"
#include <stdbool.h>


typedef enum	{
	pinOutputType_PushPull,
	pinOutputType_OpenDrain
} pinOutputType_t;

typedef struct
{
	GPIO_TypeDef *gpio;
	GPIO_Pin_TypeDef pin;
} pinOutput_t;

void PinOutput_Init(pinOutput_t *this, GPIO_TypeDef *gpio, GPIO_Pin_TypeDef pin, bool initState, pinOutputType_t outputType);
void PinOutput_Write(pinOutput_t *this, bool newState);
void tougle(pinOutput_t *this);