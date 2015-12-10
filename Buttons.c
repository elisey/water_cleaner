#include "Buttons.h"
#include "pin_input.h"
#include <assert.h>
#include "UptimeService.h"

#define NUM_OF_BUTTONS	(4)

typedef enum	{
	state_waitPress,
	state_pressDebounce,
	state_waitRelease,
	state_releaseDebounce
} state_t;

typedef struct	{
	pinInput_t pin;
	state_t state;
	uint32_t timer;
	uint8_t index;
} buttonData_t;

buttonData_t buttonsData[NUM_OF_BUTTONS];

button_t pressedButton;

static void processButton(buttonData_t *ptrButtonData);

void Buttons_Init()
{
	PinInput_Init( &(buttonsData[buttonUp].pin), GPIOC, GPIO_PIN_1, pinPullState_PullUp);
	PinInput_Init( &(buttonsData[buttonDown].pin), GPIOC, GPIO_PIN_4, pinPullState_PullUp);
	PinInput_Init( &(buttonsData[buttonCancel].pin), GPIOC, GPIO_PIN_3, pinPullState_PullUp);
	PinInput_Init( &(buttonsData[buttonOk].pin), GPIOC, GPIO_PIN_2, pinPullState_PullUp);
	
	uint8_t i;
	for(i=0; i < NUM_OF_BUTTONS; i++)	{
		buttonsData[i].state = state_waitPress;
		buttonsData[i].timer = 0;
		buttonsData[i].index = i;
	}
	pressedButton = buttonNone;
}

void Buttons_Process()
{
	uint8_t i;
	
	for(i=0; i < NUM_OF_BUTTONS; i++)	{
		processButton( &(buttonsData[i]) );
	}
}

button_t Buttons_getPressedButton()
{
	if (pressedButton != buttonNone)	{
		button_t button = pressedButton;
		pressedButton = buttonNone;
		return button;
	}
	return buttonNone;
}

static void processButton(buttonData_t *ptrButtonData)
{
	state_t state = ptrButtonData->state;
	
	if (state == state_waitPress)	{
		if (PinInput_Read( &(ptrButtonData->pin) ) == false)	{
			ptrButtonData->state = state_pressDebounce;
			ptrButtonData->timer = UptimeService_GetUptime();
			return;
		}
	}
	else if (state == state_pressDebounce)	{
		if (PinInput_Read( &(ptrButtonData->pin) ) == true)	{
			ptrButtonData->state = state_waitPress;
			return;
		}
		
		if (UptimeService_GetTimeDiff(ptrButtonData->timer) >= 20)	{
			//Кнопка нажата
			pressedButton = ptrButtonData->index;
			ptrButtonData->state = state_waitRelease;
			return;
		}
	}
	else if (state == state_waitRelease)	{
		if (PinInput_Read( &(ptrButtonData->pin) ) == true)	{
			ptrButtonData->state = state_releaseDebounce;
			ptrButtonData->timer = UptimeService_GetUptime();
			return;
		}
	}
	else if (state == state_releaseDebounce)	{
		if (UptimeService_GetTimeDiff(ptrButtonData->timer) >= 10)	{
			ptrButtonData->state = state_waitPress;
			return;
		}
	}
	else	{
		assert(0);
	}
}