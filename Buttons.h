#pragma once

#include <stdint.h>

typedef enum	{
	buttonUp = 0,
	buttonDown = 1,
	buttonCancel = 2,
	buttonOk = 3
} button_t;

void Buttons_Init();
void Buttons_Process();