#pragma once
#include <stdint.h>
void UptimeService_Init();
uint32_t UptimeService_GetUptime();
uint32_t UptimeService_GetTimeDiff(uint32_t prevTime);