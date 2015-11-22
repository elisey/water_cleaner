#include "pin_output.h"

void PinOutput_Init(pinOutput_t *this, GPIO_TypeDef *gpio, GPIO_Pin_TypeDef pin, bool initState, pinOutputType_t outputType)
{
	this->gpio = gpio;
	this->pin = pin;
	
	GPIO_Mode_TypeDef type;
	
	if (outputType == pinOutputType_PushPull)	{
		type = GPIO_MODE_OUT_PP_LOW_FAST;
	}
	else	{
		type = GPIO_MODE_OUT_OD_LOW_FAST;
	}
	
	GPIO_Init(gpio, pin, type);
	
	PinOutput_Write(this, initState);
}

void PinOutput_Write(pinOutput_t *this, bool newState)
{
	if (newState == true)	{
		GPIO_WriteHigh(this->gpio, this->pin);
	}
	else	{
		GPIO_WriteLow(this->gpio, this->pin);
	}
}

void tougle(pinOutput_t *this)
{
	GPIO_WriteReverse(this->gpio, this->pin);
}
