#include "pin_input.h"

void PinInput_Init(pinInput_t *this, GPIO_TypeDef *gpio, GPIO_Pin_TypeDef pin, pinPullState_t pullState)
{
	this->gpio = gpio;
	this->pin = pin;
	
	GPIO_Mode_TypeDef type;
	if (pullState == pinPullState_Floating)	{
		type = GPIO_MODE_IN_FL_NO_IT;
	}
	else	{
		type = GPIO_MODE_IN_PU_NO_IT;
	}
	GPIO_Init(gpio, pin, type);
}

bool PinInput_Read(pinInput_t *this)
{
	uint8_t portValue = GPIO_ReadInputData(this->gpio);
	return (portValue & this->pin);
}