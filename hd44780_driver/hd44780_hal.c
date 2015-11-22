#include "hd44780_hal.h"
#include <iostm8s005k6.h>
#include "gpio.h"

#define LCD_D4_GPIO		GPIOB
#define LCD_D4_PIN		GPIO_PIN_2

#define LCD_D5_GPIO		GPIOB
#define LCD_D5_PIN		GPIO_PIN_1

#define LCD_D6_GPIO		GPIOB
#define LCD_D6_PIN		GPIO_PIN_0

#define LCD_D7_GPIO		GPIOE
#define LCD_D7_PIN		GPIO_PIN_5

#define LCD_E_GPIO		GPIOB
#define LCD_E_PIN		GPIO_PIN_3

#define LCD_RW_GPIO		GPIOB
#define LCD_RW_PIN		GPIO_PIN_4

#define LCD_RS_GPIO		GPIOB
#define LCD_RS_PIN		GPIO_PIN_5

#define bit(n)							(1u << (n))
#define setBit(x,n)           			((x) |= bit(n))
#define clrBit(x,n)  		  			((x) &= ~bit(n))
#define getBit(x,n)   		  			(((x) & bit(n)) ? 1u : 0u)

void LcdHal_GpioInit()
{
	GPIO_Init(LCD_D4_GPIO, LCD_D4_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(LCD_D5_GPIO, LCD_D5_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(LCD_D6_GPIO, LCD_D6_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(LCD_D7_GPIO, LCD_D7_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	
	GPIO_Init(LCD_E_GPIO, LCD_E_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(LCD_RW_GPIO, LCD_RW_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(LCD_RS_GPIO, LCD_RS_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
}

void LcdHal_SetBusAsOutput()
{
	GPIO_Init(LCD_D4_GPIO, LCD_D4_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(LCD_D5_GPIO, LCD_D5_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(LCD_D6_GPIO, LCD_D6_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(LCD_D7_GPIO, LCD_D7_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
}

void LcdHal_SetBusAsInput()
{
	GPIO_Init(LCD_D4_GPIO, LCD_D4_PIN, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(LCD_D5_GPIO, LCD_D5_PIN, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(LCD_D6_GPIO, LCD_D6_PIN, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(LCD_D7_GPIO, LCD_D7_PIN, GPIO_MODE_IN_PU_NO_IT);
}

void LcdHal_SetD7AsOutput()
{
	GPIO_Init(LCD_D7_GPIO, LCD_D7_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
}

void LcdHal_SetD7AsInput()
{
	GPIO_Init(LCD_D7_GPIO, LCD_D7_PIN, GPIO_MODE_IN_PU_NO_IT);
}

void LcdHal_WriteBus(uint8_t data)
{
	if(data & 0x8)	{
		GPIO_WriteHigh(LCD_D7_GPIO, LCD_D7_PIN);
	}
	else	{
		GPIO_WriteLow(LCD_D7_GPIO, LCD_D7_PIN);
	}

	if(data & 0x4)	{
		GPIO_WriteHigh(LCD_D6_GPIO, LCD_D6_PIN);
	}
	else	{
		GPIO_WriteLow(LCD_D6_GPIO, LCD_D6_PIN);
	}

	if(data & 0x2)	{
		GPIO_WriteHigh(LCD_D5_GPIO, LCD_D5_PIN);
	}
	else	{
		GPIO_WriteLow(LCD_D5_GPIO, LCD_D5_PIN);
	}

	if(data & 0x1)	{
		GPIO_WriteHigh(LCD_D4_GPIO, LCD_D4_PIN);
	}
	else	{
		GPIO_WriteLow(LCD_D4_GPIO, LCD_D4_PIN);
	}
}

uint8_t LcdHal_ReadBus()
{
	uint8_t data = 0;

	if (GPIO_ReadInputPin(LCD_D4_GPIO, LCD_D4_PIN) == SET)	{
		setBit(data, 0);
	}
	if (GPIO_ReadInputPin(LCD_D5_GPIO, LCD_D5_PIN) == SET)	{
		setBit(data, 1);
	}
	if (GPIO_ReadInputPin(LCD_D6_GPIO, LCD_D6_PIN) == SET)	{
		setBit(data, 2);
	}
	if (GPIO_ReadInputPin(LCD_D7_GPIO, LCD_D7_PIN) == SET)	{
		setBit(data, 3);
	}
	return data;
}

bool LcdHal_ReadD7()
{
	if (GPIO_ReadInputPin(LCD_D7_GPIO, LCD_D7_PIN) == SET)	{
		return true;
	}
	return false;
}

void LcdHal_SetE()
{
	GPIO_WriteHigh(LCD_E_GPIO, LCD_E_PIN);
}

void LcdHal_ResetE()
{
	GPIO_WriteLow(LCD_E_GPIO, LCD_E_PIN);
}

void LcdHal_SetRW()
{
	GPIO_WriteHigh(LCD_RW_GPIO, LCD_RW_PIN);
}

void LcdHal_ResetRW()
{
	GPIO_WriteLow(LCD_RW_GPIO, LCD_RW_PIN);
}

void LcdHal_SetRS()
{
	GPIO_WriteHigh(LCD_RS_GPIO, LCD_RS_PIN);
}

void LcdHal_ResetRS()
{
	GPIO_WriteLow(LCD_RS_GPIO, LCD_RS_PIN);
}
