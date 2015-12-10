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

void DisplayDriver_PutString(uint8_t line, uint8_t colomn, char *str)
{
	while (*str != '\0')	{
		DisplayDriver_PutChar(line, colomn, *str);
		str++;	
		colomn++;
		if (colomn >= NUM_OF_COLOMNS)	{
			colomn = 0;
			line++;
			if (line >= NUM_OF_LINES)	{
				line = 0;
			}
		}
	}
}

void DisplayDriver_Clear()
{
	int i;
	for(i=0; i < VIDEO_BUFFER_SIZE; i++)	{
		videoBuffer[i] = 0x20;
		videoBufferCopy[i] = 0;
	}
}

char* itoa(int i, char b[])
{
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}