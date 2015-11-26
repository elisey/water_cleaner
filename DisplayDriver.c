#include "DisplayDriver.h"
#include "UptimeService.h"
#include "hd44780.h"

#define NUM_OF_LINES		(2)
#define NUM_OF_COLOMNS		(16)
#define VIDEO_BUFFER_SIZE	(NUM_OF_LINES * NUM_OF_COLOMNS)

static uint8_t videoBuffer[VIDEO_BUFFER_SIZE];

void DisplayDriver_Init()
{
	Lcd_Init();
	
	int i;
	for(i=0; i < VIDEO_BUFFER_SIZE; i++)	{
		videoBuffer[i] = 0x20;
	}
}

void DisplayDriver_Process()
{
	static uint32_t prevRefleshTime = 0;
	
	if (UptimeService_GetTimeDiff(prevRefleshTime) > 10)	{
		prevRefleshTime = UptimeService_GetUptime();
		
		Lcd_Goto(1,0);
		
		int i;
		for(i=0; i < VIDEO_BUFFER_SIZE; i++)	{
			
			if (i == NUM_OF_COLOMNS)	{
				Lcd_Goto(2,0);
			}
			
			Lcd_PutChar(videoBuffer[i]);
		}
	}
}

void DisplayDriver_PutChar(uint8_t line, uint8_t coloumn, uint8_t ch)
{
	uint8_t videoDriverIndex = (line * NUM_OF_COLOMNS) + coloumn;
	
	videoBuffer[videoDriverIndex] = ch;
}