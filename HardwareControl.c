#include "HardwareControl.h"

#include "pin_input.h"
#include "pin_output.h"

#include <assert.h>

pinOutput_t relays[7];
pinInput_t inputs[3];

void HardwareControl_Init()
{
	PinOutput_Init(&(relays[0]), GPIOD, GPIO_PIN_7, false, pinOutputType_PushPull);
	PinOutput_Init(&(relays[1]), GPIOD, GPIO_PIN_6, false, pinOutputType_PushPull);
	PinOutput_Init(&(relays[2]), GPIOD, GPIO_PIN_5, false, pinOutputType_PushPull);
	PinOutput_Init(&(relays[3]), GPIOD, GPIO_PIN_4, false, pinOutputType_PushPull);
	PinOutput_Init(&(relays[4]), GPIOD, GPIO_PIN_3, false, pinOutputType_PushPull);
	PinOutput_Init(&(relays[5]), GPIOD, GPIO_PIN_2, false, pinOutputType_PushPull);
	PinOutput_Init(&(relays[6]), GPIOD, GPIO_PIN_0, false, pinOutputType_PushPull);
	
	PinInput_Init(&(inputs[0]), GPIOC, GPIO_PIN_7, pinPullState_PullUp);
	PinInput_Init(&(inputs[1]), GPIOC, GPIO_PIN_6, pinPullState_PullUp);
	PinInput_Init(&(inputs[2]), GPIOC, GPIO_PIN_5, pinPullState_PullUp);
}

void HardwareControl_SetOutputState(uint8_t outputIndex, bool newState)
{
	assert(outputIndex < 7);
	PinOutput_Write( &(relays[outputIndex]), newState);
}

bool HardwareControl_GetInputState(uint8_t inputIndex)
{
	assert(inputIndex < 3);
	return !(PinInput_Read( &(inputs[inputIndex]) ) );
}