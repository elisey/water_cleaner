#pragma once

#include <stdbool.h>
#include <stdint.h>

void HardwareControl_Init();
void HardwareControl_SetOutputState(uint8_t outputIndex, bool newState);
bool HardwareControl_GetInputState(uint8_t inputIndex);