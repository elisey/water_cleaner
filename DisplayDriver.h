#pragma once
#include <stdint.h>

void DisplayDriver_Init();
void DisplayDriver_Process();

void DisplayDriver_PutChar(uint8_t line, uint8_t coloumn, uint8_t ch);
void DisplayDriver_PutString(uint8_t line, uint8_t coloumn, char *str);
void DisplayDriver_Clear();
char* itoa(int i, char b[]);