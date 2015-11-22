//------------------------------------------------------
/* File:       Library for HD44780 compatible displays  */
/* Version:	   v2.01  						 			*/
/* Author:     GrAnd/www.MakeSystem.net		 			*/
/* Tested on:  AVR, STM32F10X		 	 	 		 	*/
/* License:	   GNU LGPLv2.1		 		 	 			*/
//------------------------------------------------------
/* Copyright (C)2014 GrAnd. All right reserved 			*/
//------------------------------------------------------

/*
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Contact information :
						mail@makesystem.net
						http://makesystem.net/?page_id=2
*/

#include "hd44780.h"
#include "hd44780_hal.h"
#include "stdbool.h"

#define ENABLE_CYCLE_TIME			1u	/* Minimal value ~ 1us */
#define AC_UPDATE_TIME				1u	/* Minimal value ~ 4us */

#if (USE_PROGRESS_BAR)
/*!	\brief	Progress bar definitions. */
#define CGROM_PROGRESS_BAR_SIZE		6u
#define FULL_LOAD					5u
#define EMPTY_LOAD					0u
#define EMPTY_ROW					0x00u /* xxx00000 */

static const uint8_t progress_bar[CGROM_PROGRESS_BAR_SIZE] = {0x00u,0x10u,0x18u,0x1Cu,0x1Eu,0x1Fu};
static uint8_t current_bar_pixel;
static uint8_t current_cell_load;

static void prv_initbar(void);
#endif

static void prv_config(uint8_t param);
static void prv_write(uint8_t data);
static uint8_t prv_read();
static void prv_strobe(void);
static void prv_send_high_nibble(uint8_t data);
static void prv_send_low_nibble(uint8_t data);
static void prv_10us_delay(volatile uint32_t us);
static uint32_t prv_pow10(uint8_t n);
#if (USE_CYRILLIC_SYMBOLS == 1)
static uint8_t prv_decode_cyrillic_symbol(uint8_t symbol);
#endif
#if (USE_BUSY_FLAG)
static void prv_busy_delay(void);
#endif
//#if (USE_CYRILLIC_SYMBOLS == 1)
const uint8_t cyrillicTable[255 - 192 + 1] = { 0x41, 0xA0, 0x42, 0xA1, 0xE0, 0x45,
		0xA3, 0xA4, 0xA5, 0xA6, 0x4B, 0xA7, 0x4D, 0x48, 0x4F, 0xA8, 0x50, 0x43,
		0x54, 0xA9, 0xAA, 0x58, 0xE1, 0xAB, 0xAC, 0xE2, 0xAD, 0xAE, 0xAD, 0xAF,
		0xB0, 0xB1, 0x61, 0xB2, 0xB3, 0xB4, 0xE3, 0x65, 0xB6, 0xB7, 0xB8, 0xB9,
		0xBA, 0xBB, 0xBC, 0xBD, 0x6F, 0xBE, 0x70, 0x63, 0xBF, 0x79, 0xE4, 0x78,
		0xE5, 0xC0, 0xC1, 0xE6, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7 };
//#endif

static void prv_busy_delay(void)
{
	bool BusyFlag;

	LcdHal_SetD7AsInput();
	/* When RS = 0 and R/W = 1, the busy flag is output to DB7. */
	LcdHal_ResetRS();
	LcdHal_SetRW();

	do
	{/* Note: two cycles are needed for the busy flag check. */
		/* Read busy flag. */
		prv_strobe();
		/* D7 is used as busy flag. */
		BusyFlag = LcdHal_ReadD7();
		/* Discard D3. */
		prv_strobe();
		/* Verify the busy flag */
	}while (BusyFlag);

	LcdHal_ResetRW();
	LcdHal_SetD7AsOutput();
}
//#endif /* USE_BUSY_FLAG */

/*!	\brief	Creates delay multiples of 10us. */
static void prv_10us_delay(volatile uint32_t us)
{
	us *= MCU_FREQ_VALUE;
	while (us > 0u)	{
		us--;
	}
}

/*!	\brief	Initiate the transfer of data/commands to LCD. */
static void prv_strobe(void)
{
	LcdHal_SetE();
	prv_10us_delay(ENABLE_CYCLE_TIME);
	LcdHal_ResetE();
}

/*!	\brief	Send the msb nibble of the data / command to LCD. */
static void prv_send_high_nibble(uint8_t data)
{
	LcdHal_WriteBus((data >> 4) & 0xF);
}

/*!	\brief	Send the lsb nibble of the data / command to LCD. */
static void prv_send_low_nibble(uint8_t data)
{
	LcdHal_WriteBus(data & 0xF);
}

/*!	\brief	Send data/commands to the display. */
static void prv_write(uint8_t data)
{
#if (USE_BUSY_FLAG)
	LcdHal_ResetRW();
#endif /* USE_BUSY_FLAG */

	prv_send_high_nibble(data);
	prv_strobe();
	prv_send_low_nibble(data);
	prv_strobe();

	/* The busy flag must be checked after the 4-bit data has been transferred twice. */
#if (USE_BUSY_FLAG)
	prv_busy_delay();
#else
	prv_10us_delay(BUSY_CYCLE_TIME);
#endif /* USE_BUSY_FLAG */
}

static uint8_t prv_read()
{
	LcdHal_SetBusAsInput();
	LcdHal_SetRW();
	LcdHal_SetE();

	prv_10us_delay(ENABLE_CYCLE_TIME);

	uint8_t data = 0;
	data |= (LcdHal_ReadBus() << 4);
	LcdHal_ResetE();
	prv_10us_delay(ENABLE_CYCLE_TIME);
	LcdHal_SetE();
	prv_10us_delay(ENABLE_CYCLE_TIME);
	data |= LcdHal_ReadBus();
	LcdHal_ResetE();
	LcdHal_SetBusAsOutput();
	return data;
}

/*!	\brief	Initializing by instruction. 4-bit interface initialization. */
static void prv_config(uint8_t param)
{/* Low level function. */
	/* Send commands to LCD. */
	LcdHal_ResetRS();
#if (USE_BUSY_FLAG)
	/* Write data/commands to LCD. */
	LcdHal_ResetRW();
#endif /* USE_BUSY_FLAG */

	prv_send_high_nibble(param);
	prv_strobe();		// Change 8-bit interface to 4-bit interface
	prv_10us_delay(BUSY_CYCLE_TIME);
	prv_strobe();		/* DB7 to DB4 of the "Function set" instruction is written twice. */
	prv_10us_delay(BUSY_CYCLE_TIME);
	prv_send_low_nibble(param);
	prv_strobe();		// 4-bit, two lines, 5x8 pixel
	prv_10us_delay(BUSY_CYCLE_TIME);
	/* Note: The number of display lines and character font cannot be changed after this point. */
}

							//-----------------------------//
							/*         LCDlib API          */
							//-----------------------------//

/*!	\details	Clear display writes space code 20H into all DDRAM addresses.
 * 				It then sets DDRAM address 0 into the address counter,
 * 				and returns the display to its original status if it was shifted.
 * 				In other words, the display disappears and the cursor
 * 				or blinking goes to the left edge of the display (in the first line if 2 lines are displayed).
 * 				It also sets I/D to 1 (increment mode) in entry mode (S of entry mode does not change). */
void Lcd_ClearScreen(void)
{
	/* Send a command to LCD. */
	LcdHal_ResetRS();
	/* Clear screen */
	prv_write(0x01u);
	/* Busy delay */
#if (USE_BUSY_FLAG)
	prv_busy_delay();
#else
	prv_10us_delay(CLRSCR_CYCLE_TIME);
#endif /* USE_BUSY_FLAG */
}

/*!	\details	"Return home" sets DDRAM address 0 into the address counter,
 * 				and returns the display to its original status if it was shifted.
 * 				The DDRAM contents do not change.
 * 				The cursor or blinking go to the left edge of the display
 * 				(in the first line if 2 lines are displayed). */
void Lcd_Return(void)
{
	/* Send a command to LCD. */
	LcdHal_ResetRS();
	/* Return home */
	prv_write(0x02u);
	/* Busy delay */
#if (USE_BUSY_FLAG)
	prv_busy_delay();
#else
	prv_10us_delay(RETHOME_CYCLE_TIME);
#endif /* USE_BUSY_FLAG */
}

/*!	\details	lcd_scroll shifts the display to the right or left without writing or reading display data.
 * 				This function is used to correct or search the display.
 *	\note		The first and second line displays will shift at the same time. */
void Lcd_Scroll(uint8_t direction)
{
	/* Send a command to LCD. */
	LcdHal_ResetRS();
	/* Scroll display */
	switch (direction)
	{
	/* To left */
		case LEFT  :
			prv_write(0x18u);
			break;

		/* To right */
		case RIGHT :
			prv_write(0x1Cu);
			break;

		default:
			/* Ignore this command */
			break;
	}
}

/*!	\details	"Cursor shift" shifts the cursor position to the right or left,
 * 				without writing or reading display data.
 * 				This function is used to correct or search the display.
 * 				In a 2-line display, the cursor moves to the second line
 * 				when it passes the 40th digit of the first line. */
void Lcd_CursorShift(uint8_t direction)
{
	/* Send a command to LCD. */
	LcdHal_ResetRS();
	/* Shift cursor */
	switch (direction)
	{
		/* To left */
		case LEFT  :
			prv_write(0x10u);
			break;

		/* To right */
		case RIGHT :
			prv_write(0x14u);
			break;

		default:
			/* Ignore this command */
			break;
	}
}

/*!	\details	Go to the specified (DDRAM/CGRAM) memory address.*/
void Lcd_Goto(uint8_t line, uint8_t address)
{
	/* Send a command to LCD. */
	LcdHal_ResetRS();
	/* Set DDRAM/CGRAM address. */
	switch (line)
	{
		/* Set DDRAM address. */
		case LCD_1st_LINE: prv_write(0x80u | START_ADDRESS_1st_LINE | address); break;
		case LCD_2nd_LINE: prv_write(0x80u | START_ADDRESS_2nd_LINE | address); break;
		case LCD_3rd_LINE: prv_write(0x80u | START_ADDRESS_3rd_LINE | address); break;
		case LCD_4th_LINE: prv_write(0x80u | START_ADDRESS_4th_LINE | address); break;
		/* Set CGRAM address. */
		case CGRAM : prv_write(0x40u | address); break;

		default:
			break;
	}
}

/*!	\details	Change LCD settings. */
void Lcd_SetMode(uint8_t param)
{
	/* Send a command to LCD. */
	LcdHal_ResetRS();
	prv_write(param);
}

/*!	\details	Write a single char to the current memory space (DDRAM/CGRAM). */
void Lcd_PutChar(uint8_t data)
{
#if (USE_CYRILLIC_SYMBOLS == 1)
	data = prv_decode_cyrillic_symbol(data);
#endif
	/* Send data to LCD. */
	LcdHal_SetRS();
	prv_write(data);
	/* Note:
	 * After execution of the CGRAM/DDRAM data write/read instruction, the RAM address counter is incremented
	 * or decremented by 1. The RAM address counter is updated after the busy flag turns off.
	 * tADD is the time elapsed after the busy flag turns off until the address counter is updated. */
	prv_10us_delay(AC_UPDATE_TIME);	/* Update RAM address counter delay. */
}

uint8_t Lcd_ReadChar()
{
	LcdHal_SetRS();
	uint8_t data = prv_read();
	return data;
}

/*!	\details	Writes ANSI-C string to LCD (DDRAM memory space). */
void Lcd_PutString(const char *str)
{
	/* Send a ANSI-C string to LCD. */
	while ('\0' != *str)
	{
#if ( USE_FORMATTED_OUTPUT )
		if(('\n' == *str))
		{
			Lcd_Goto(LCD_2nd_LINE, 0u);
		}
		else if(('\r' == *str))
		{
			Lcd_Return();
		}
		else if(('\t' == *str))
		{
			uint8_t i;

			for(i=0u; i<TAB_SPACE; i++)
			{
				Lcd_CursorShift(RIGHT);
			}
		}
		else
#endif
		{
			Lcd_PutChar(*str);
		}
		str++;
	}
}

/*!	\details	Load the user-defined symbol into the CGRAM memory. */
void Lcd_LoadUserChar(uint8_t* vector, uint8_t position)
{
	uint8_t i;
	/* Go to the CGRAM memory space: 0 to 7 */
	Lcd_Goto(CGRAM, (position * FONT_HEIGHT));

	for(i = 0u; i < FONT_HEIGHT; i++)
	{/* Load one row of pixels into the CGRAM register. */
		Lcd_PutChar(vector[i]);
	}

	/* Return to the DDRAM memory space. */
	Lcd_Goto(LCD_1st_LINE, 0u);
}

/*!	\details	Load and display the user-defined symbol. */
void Lcd_LoadAndDrawUserChar( uint8_t* vector,
			   	   uint8_t position,
			   	   uint8_t line,
			   	   uint8_t address )
{
	/* Load the user-defined symbol into the CGRAM memory. */
	Lcd_LoadUserChar(vector, position);
	/* Select LCD position. */
	Lcd_Goto(line, address);
	/* Display the user-defined symbol. */
	Lcd_PutChar(position);
}

/*!	\details	Erase a symbol from the left of the cursor. */
void Lcd_Backspace(void)
{
	Lcd_CursorShift(LEFT);		// �������� ������ �� ���� ������� �����
	Lcd_PutChar(' ');			// �������, ����� ���� ���������� ������������� ������
	Lcd_CursorShift(LEFT);		// �������� ������ �� ���� ������� �����
}

/*!	\brief	Returns 10^n value. */
static uint32_t prv_pow10(uint8_t n)
{
	uint32_t retval = 1u;

	while (n > 0u)
	{
		retval *= 10u;
		n--;
	}

	return retval;
}

/*!	\brief	Display a integer number: +/- 2147483647. */
void Lcd_itos(int32_t value)
{
	int32_t i;

	if (value < 0)
	{
		Lcd_PutChar('-');
		value = -value;
	}

	i = 1;
	while ((value / i) > 9)
	{
		i *= 10;
	}

	Lcd_PutChar(value/i + '0');	/* Display at least one symbol */
	i /= 10;

	while (i > 0)
	{
		Lcd_PutChar('0' + ((value % (i*10)) / i));
		i /= 10;
	}
}

/*!	\brief	Display a floating point number. */
void Lcd_ftos(float value, uint8_t n)
{
	if (value < 0.0)
	{
		Lcd_PutChar('-');
		value = -value;
	}

	Lcd_itos((int32_t)value); // ����� ����� �����

	if (n > 0u)
	{
		Lcd_PutChar('.'); // �����

		Lcd_ntos((uint32_t)(value * (float)prv_pow10(n)), n); // ����� ������� �����
	}
}

/*!	\brief	Display "n" right digits of "value". */
void Lcd_ntos(uint32_t value, uint8_t n)
{
	if (n > 0u)
	{
		uint32_t i = prv_pow10(n - 1u);

		while (i > 0u)	/* Display at least one symbol */
		{
			Lcd_PutChar('0' + ((value/i) % 10u));

			i /= 10u;
		}
	}
}

#if ( USE_PROGRESS_BAR )
/*!	\brief	Initialize the progress bar
 * 			(i.e. preload elements of the progress bar into CGRAM and reset all variables). */
static void prv_initbar(void)
{
	uint8_t i, j;

	for (i = 0u; i < CGROM_PROGRESS_BAR_SIZE; i++)
	{
		Lcd_Goto(CGRAM, (i * FONT_HEIGHT));

		for (j = 0u; j < FONT_HEIGHT; j++)
		{
			if (j < PROGRESS_BAR_HEIGHT)
			{
				Lcd_PutChar(progress_bar[i]);
			}
			else
			{/* Load an empty row of pixels in CGRAM. */
				Lcd_PutChar(EMPTY_ROW);
			}
		}
	}

	/* Clear the entire bar and initialize all variables. */
	Lcd_ClearBar();
}

/*!	\brief	Draw progress bar. */
void Lcd_DrawBar(uint8_t next_bar_pixel)
{
	/* Go to the current cell position in the progress bar. */
	Lcd_Goto(PROGRESS_BAR_LINE, (current_bar_pixel / FONT_WIDTH));

	if (next_bar_pixel > current_bar_pixel)
	{
		/* Increment LCD cursor */
		Lcd_SetMode(ENTRY_MODE_INC_NO_SHIFT);

		/* Prevent the progress bar overflow */
		if (next_bar_pixel > PROGRESS_BAR_MAX_LOAD)
		{
			next_bar_pixel = PROGRESS_BAR_MAX_LOAD;
		}

		while (current_bar_pixel != next_bar_pixel)
		{
			/* Go to the next pixel. */
			current_bar_pixel++;
			current_cell_load++;
			/* Display the load of the current cell. */
			Lcd_PutChar(current_cell_load);

			if (current_cell_load < FULL_LOAD)
			{/* Return the cursor to the current cell. */
				Lcd_CursorShift(LEFT);
			}
			else
			{/* Go to the next cell. */
				current_cell_load = EMPTY_LOAD;
			}
		}
	 }
#if (USE_REGRESS_BAR)
	 else if (next_bar_pixel < current_bar_pixel)
	 {
		 /* Decrement LCD cursor */
		Lcd_SetMode(ENTRY_MODE_DEC_NO_SHIFT);

		do
		{
			if (EMPTY_LOAD == current_cell_load)
			{/* Go to the next cell. */
				Lcd_CursorShift(LEFT);
				current_cell_load = FULL_LOAD;
			}
			/* Go to the next pixel. */
			current_bar_pixel--;
			current_cell_load--;
			/* Display the load of the current cell. */
			Lcd_PutChar(current_cell_load);
			/* Return the cursor to the current cell. */
			Lcd_CursorShift(RIGHT);
		}
		while (current_bar_pixel != next_bar_pixel);
	 }
#endif /* USE_REGRESS_BAR */
	 else
	 {
		 /* Nothing to do. */
	 }

	/* Restore the default entry mode. */
	Lcd_SetMode(DEFAULT_ENTRY_MODE);
	/* Return home. */
	Lcd_Goto(LCD_1st_LINE, 0u);
}

/*!	\brief	Clear the entire progress bar. */
void Lcd_ClearBar(void)
{
	uint8_t i;
	/* Go to the last cell in the progress bar. */
	Lcd_Goto(PROGRESS_BAR_LINE, (PROGRESS_BAR_WIDTH - 1u));
	/* Set the decrement mode. */
	Lcd_SetMode(ENTRY_MODE_DEC_NO_SHIFT);

	for(i = 0u; i < PROGRESS_BAR_WIDTH; i++)
	{/* Display the "empty cell" symbol (i.e. clear the LCD cell). */
		Lcd_PutChar(EMPTY_LOAD);
	}

	/* Reset the progress bar variables. */
	current_bar_pixel = 0u;
	current_cell_load = EMPTY_LOAD;

	/* Restore the default entry mode. */
	Lcd_SetMode(DEFAULT_ENTRY_MODE);
	/* Return home. */
	Lcd_Goto(LCD_1st_LINE, 0u);
}
#endif

/*!	\brief	Initialize the LCD.
 * 	\note	This library use the 4-bit interface. */
void Lcd_Init(void)
{
	LcdHal_GpioInit();

	Lcd_SetMode(0x30);
	prv_10us_delay(1000);
	Lcd_SetMode(0x30);
	prv_10us_delay(200);
	Lcd_SetMode(0x30);

	

	prv_config(DEFAULT_DISPLAY_CONFIG);
	Lcd_SetMode(DEFAULT_VIEW_MODE);
	Lcd_SetMode(DEFAULT_ENTRY_MODE);
	Lcd_ClearScreen();
#if (USE_PROGRESS_BAR)
	prv_initbar();
#endif
}

#if (USE_CYRILLIC_SYMBOLS == 1)
static uint8_t prv_decode_cyrillic_symbol(uint8_t symbol)
{
	if(symbol >= 192)	{
		return (cyrillicTable [ symbol - 192 ]);
	}
	else	{
		return symbol;
	}
}
#endif
