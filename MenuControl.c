#include "MenuControl.h"
#include "Buttons.h"
#include "hd44780.h"
#include "UptimeService.h"
#include "DisplayDriver.h"
#include <stdbool.h>

#include "Automation.h"
#include <string.h>

typedef struct	{
	void (*ptrRedrowFunc)();
	void (*ptrButtonProcessFunc)(button_t buttonsId);
} window_t;

window_t windows[5];
uint8_t currentWindow = 0;

void WindowMain_redrow();
void WindowMain_buttonProcess(button_t buttonsId);

void WindowSettings_redrow();
void WindowSettings_buttonProcess(button_t buttonsId);

void WindowStageSet_redrow();
void WindowStageSet_buttonProcess(button_t buttonsId);

void WindowTimeSet_redrow();
void WindowTimeSet_buttonProcess(button_t buttonsId);

void WindowTimeEnter_redrow();
void WindowTimeEnter_buttonProcess(button_t buttonsId);
static void clearEditValueSettings();

void MenuControl_Init()
{
	currentWindow = 0;
	windows[0].ptrRedrowFunc = WindowMain_redrow;
	windows[0].ptrButtonProcessFunc = WindowMain_buttonProcess;
	
	windows[1].ptrRedrowFunc = WindowSettings_redrow;
	windows[1].ptrButtonProcessFunc = WindowSettings_buttonProcess;
	
	windows[2].ptrRedrowFunc = WindowStageSet_redrow;
	windows[2].ptrButtonProcessFunc = WindowStageSet_buttonProcess;
	
	windows[3].ptrRedrowFunc = WindowTimeSet_redrow;
	windows[3].ptrButtonProcessFunc = WindowTimeSet_buttonProcess;
	
	windows[4].ptrRedrowFunc = WindowTimeEnter_redrow;
	windows[4].ptrButtonProcessFunc = WindowTimeEnter_buttonProcess;
}

void MenuControl_Process()
{
	static uint32_t prevRefleshTime = 0;
	bool needReflesh = false;
	
	button_t pressedButton;
	pressedButton = Buttons_getPressedButton();
	
	
	
	if (pressedButton != buttonNone)	{
		windows[currentWindow].ptrButtonProcessFunc(pressedButton);
		needReflesh = true;
	}
	
	if ( (UptimeService_GetTimeDiff(prevRefleshTime) > 20) || (needReflesh) )	{
		needReflesh = false;
		prevRefleshTime = UptimeService_GetUptime();

		windows[currentWindow].ptrRedrowFunc();
	}
}


void WindowMain_redrow()
{
	DisplayDriver_Clear();
	
	uint8_t currentState = Automation_GetCurrentState();
	
	uint32_t timeInState = Automation_GetTimeInState() / 1000;
	uint32_t fullTimeInState = Automation_GetFullTimeInState(currentState) / 1000;
	
	if (timeInState == fullTimeInState)	{
		timeInState--;
	}
	
	if (currentState == 0)	{
		DisplayDriver_PutString(0, 0, "Остановлен");
		DisplayDriver_PutString(0, 11, "Старт");
		return;
	}
	
	
	DisplayDriver_PutString(1, 12, "Стоп");
	
	DisplayDriver_PutString(0, 0, "Этап");
	char string[16];
	
	itoa(currentState, string);
	DisplayDriver_PutString(0, 5, string);

	if (currentState == 1)	{
		DisplayDriver_PutString(1, 0, "Ожидание");
		return;
	}
	
	itoa(timeInState, string);
	DisplayDriver_PutString(1, 0, string);
	
	uint8_t lenght = strlen(string);
	DisplayDriver_PutString(1, lenght, "c/");
	
	itoa(fullTimeInState, string);
	
	DisplayDriver_PutString(1, lenght + 2, string);
	
	lenght += strlen(string);
	
	DisplayDriver_PutChar(1, lenght + 2, 'c');
}

void WindowMain_buttonProcess(button_t buttonsId)
{
	if (buttonsId == buttonUp)	{
		currentWindow = 1;
	}
	else if (buttonsId == buttonDown)	{
		currentWindow = 1;
	}
	
	else if (buttonsId == buttonOk)	{
		Automation_SetState(1);
	}
	else if (buttonsId == buttonCancel)	{
		Automation_Stop();
	}
}

/*----------------------------------------------------------------------------*/
static uint8_t WindowSettingsMenuIndex = 0;

void WindowSettings_redrow()
{
	DisplayDriver_Clear();
	
	DisplayDriver_PutString(0, 2, "Настр. времени");
	DisplayDriver_PutString(1, 2, "Уст. этап");
	
	if (WindowSettingsMenuIndex == 0)	{
		DisplayDriver_PutChar(0, 0, '>');
		DisplayDriver_PutChar(1, 0, ' ');
	}
	else	{
		DisplayDriver_PutChar(0, 0, ' ');
		DisplayDriver_PutChar(1, 0, '>');
	}
}

void WindowSettings_buttonProcess(button_t buttonsId)
{
	if (buttonsId == buttonUp)	{
		if (WindowSettingsMenuIndex == 0)	{
			WindowSettingsMenuIndex = 0;
			currentWindow = 0;
		}
		else	{
			WindowSettingsMenuIndex--;
		}
	}
	else if (buttonsId == buttonDown)	{
		WindowSettingsMenuIndex++;
		
		if (WindowSettingsMenuIndex >= 2)	{
			WindowSettingsMenuIndex = 0;
			currentWindow = 0;
		}
	}
	
	else if (buttonsId == buttonOk)	{
		if (WindowSettingsMenuIndex == 1)	{
			WindowSettingsMenuIndex = 0;
			currentWindow = 2;
		}
		else if (WindowSettingsMenuIndex == 0)	{
			WindowSettingsMenuIndex = 0;
			currentWindow = 3;
			//Уставки го
		}
	}
	else if (buttonsId == buttonCancel)	{
		WindowSettingsMenuIndex = 0;
		currentWindow = 0;
	}
}


/*----------------------------------------------------------------------------*/
static uint8_t chosenStage = 1;
void WindowStageSet_redrow()
{
	DisplayDriver_Clear();
	DisplayDriver_PutString(0, 0, "Этап");
	
	char string[16];
	
	itoa(chosenStage, string);
	
	DisplayDriver_PutString(0, 5, string);
}

void WindowStageSet_buttonProcess(button_t buttonsId)
{
	if (buttonsId == buttonDown)	{
		if (chosenStage > 0)	{
			chosenStage--;
		}
	}
	else if (buttonsId == buttonUp)	{
		chosenStage++;
		
		if (chosenStage >= 11)	{
			chosenStage = 11;
		}
	}
	
	else if (buttonsId == buttonOk)	{
		Automation_SetState(chosenStage);
		currentWindow = 0;
	}
	else if (buttonsId == buttonCancel)	{
		chosenStage = 1;
		currentWindow = 1;
	}
}

/*----------------------------------------------------------------------------*/
static uint8_t chosenTime = 1;
static uint8_t timeIndex = 0;
void WindowTimeSet_redrow()
{
	DisplayDriver_Clear();
	DisplayDriver_PutString(0, 0, "Задержка");
	
	char string[16];
	
	itoa(chosenTime, string);
	
	DisplayDriver_PutString(0, 9, string);
	
	uint32_t time = Automation_GetTimeout(chosenTime - 1) / 1000;
	
	itoa(time, string);
	
	DisplayDriver_PutString(1, 0, string);
	DisplayDriver_PutChar(1, strlen(string) + 1, 'с');
}

void WindowTimeSet_buttonProcess(button_t buttonsId)
{
	if (buttonsId == buttonDown)	{
		if (chosenTime > 1)	{
			chosenTime--;
		}
	}
	else if (buttonsId == buttonUp)	{
		chosenTime++;
		
		if (chosenTime >= 11)	{
			chosenTime = 11;
		}
	}
	
	else if (buttonsId == buttonOk)	{
		timeIndex = chosenTime;
		currentWindow = 4;
	}
	else if (buttonsId == buttonCancel)	{
		chosenTime = 1;
		currentWindow = 1;
	}	
}

/*----------------------------------------------------------------------------*/

uint8_t value[4] = {0,0,0,0};
uint8_t currentEditDigit = 0;

void WindowTimeEnter_redrow()
{
	DisplayDriver_Clear();
	
	DisplayDriver_PutChar(0, 0, '#');
	
	char string[16];
	itoa(chosenTime, string);
	DisplayDriver_PutString(0, 1, string);
	
	DisplayDriver_PutChar(0, 6, value[0] + 0x30);
	DisplayDriver_PutChar(0, 7, value[1] + 0x30);
	DisplayDriver_PutChar(0, 8, value[2] + 0x30);
	DisplayDriver_PutChar(0, 9, value[3] + 0x30);
	
	DisplayDriver_PutChar(0, 11, 'c');
	
	DisplayDriver_PutChar(1, 6 + currentEditDigit, '^');
}

void WindowTimeEnter_buttonProcess(button_t buttonsId)
{
	if (buttonsId == buttonUp)	{
		value[currentEditDigit]++;
		if (value[currentEditDigit] >= 10)	{
			value[currentEditDigit] = 0;
		}
	}
	else if (buttonsId == buttonDown)	{
		if (value[currentEditDigit] > 0)	{
			value[currentEditDigit] --;
		}
		else	{
			value[currentEditDigit] = 9;
		}
	}
	
	else if (buttonsId == buttonOk)	{
		currentEditDigit++;
		if (currentEditDigit >= 4)	{
			//записть, выход
			
			uint32_t time = 1000 * value[0] + 100 * value[1] + 10 * value[2] + value[3];
			Automation_SetTimeout(timeIndex - 1, time * 1000);
			
			clearEditValueSettings();
			currentWindow = 3;
		}
	}
	else if (buttonsId == buttonCancel)	{
		if (currentEditDigit > 0)	{
			currentEditDigit --;
		}
		else	{
			clearEditValueSettings();
			currentWindow = 3;
		}
	}	
}

static void clearEditValueSettings()
{
	currentEditDigit = 0;
	int i;
	for (i=0; i < 4; i ++)	{
		value[i] = 0;
	}
}
