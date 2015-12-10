#include <iostm8s005k6.h>
#include <stdint.h>
#include "HardwareControl.h"
#include "UptimeService.h"
#include <intrinsics.h>
#include "MenuControl.h"
#include "Buttons.h"
#include "DisplayDriver.h"
#include "Automation.h"

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

	DisplayDriver_Init();
	Buttons_Init();
	MenuControl_Init();
	
	Automation_Init();
	
	Automation_Start();
	
	while(1)
	{
		Automation_Process();
		Buttons_Process();
		DisplayDriver_Process();
		MenuControl_Process();


	}
}

