#pragma once

#include <stdint.h>
#include <stdbool.h>

void LcdHal_GpioInit();
void LcdHal_SetBusAsOutput();
void LcdHal_SetBusAsInput();
void LcdHal_SetD7AsOutput();
void LcdHal_SetD7AsInput();

void LcdHal_WriteBus(uint8_t data);
uint8_t LcdHal_ReadBus();
bool LcdHal_ReadD7();

void LcdHal_SetE();
void LcdHal_ResetE();
void LcdHal_SetRW();
void LcdHal_ResetRW();
void LcdHal_SetRS();
void LcdHal_ResetRS();
