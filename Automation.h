#pragma once

#include <stdint.h>

void Automation_Init();
void Automation_Process();

void Automation_Start();
void Automation_Stop();
void Automation_SetState(uint8_t newStage);

uint32_t Automation_GetTimeout(uint8_t timeoutIndex);
void Automation_SetTimeout(uint8_t timeoutIndex, uint32_t newTimeout);

uint8_t Automation_GetCurrentState();
uint32_t Automation_GetTimeInState();
uint32_t Automation_GetFullTimeInState(uint8_t stateIndex);