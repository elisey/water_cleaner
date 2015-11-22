#include <iostm8s005k6.h>
#include <stdint.h>
#include "HardwareControl.h"
#include "UptimeService.h"
#include <intrinsics.h>
#include "hd44780.h"
void delay()
{
	volatile uint32_t i;
	for(i=0; i <100000; i++)	{}
}

void sysClockInit()
{
	CLK_CKDIVR_bit.HSIDIV = 0;
}

int main( void )
{
	sysClockInit();
	UptimeService_Init();
	HardwareControl_Init();
	__enable_interrupt();
	Lcd_Init();
	Lcd_ClearScreen();
	Lcd_Goto(1,5);
	Lcd_PutChar('h');
	Lcd_PutString("Привет");
	while(1)
	{
		int i;
		
		/*for(i=0; i < 7; i++)	{
			HardwareControl_SetOutputState(i, true);
			delay();
		}
		
		for(i=0; i < 7; i++)	{
			HardwareControl_SetOutputState(i, false);
			delay();
		}*/

	}
}

