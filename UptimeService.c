#include "UptimeService.h"
#include <iostm8s005k6.h>
#include <intrinsics.h>

static uint32_t counter = 0;

static uint32_t getCounter();

void UptimeService_Init()
{
	TIM4_SR_bit.UIF = 0;
	TIM4_PSCR = 0x7;	//Предделитель на 2^7=128   16000000/128 = 125000 Hz
	TIM4_ARR = 125;		//1kHz
	TIM4_IER_bit.UIE = 1;
	TIM4_CR1_bit.CEN = 1;
}

uint32_t UptimeService_GetUptime()
{
	return getCounter();
}

uint32_t UptimeService_GetTimeDiff(uint32_t prevTime)
{
	uint32_t _counter = getCounter();
	
	if (prevTime <= _counter)	{
		return (_counter - prevTime);
	}
	else	{
		return (UINT32_MAX - prevTime) + _counter;
	}
}

static uint32_t getCounter()
{
	__disable_interrupt();
	uint32_t _counter = counter;
	__enable_interrupt();
	
	return _counter;
}

#pragma vector=0x19 
__interrupt void tim4_ovr_irq(void)
{
	TIM4_SR_bit.UIF = 0;
	
	counter++;
}