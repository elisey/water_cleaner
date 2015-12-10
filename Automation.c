#include "Automation.h"
#include "HardwareControl.h"

#include "StateMachine.h"
#include <stdbool.h>



#define NUM_OF_TIMES	(11)
#define NUM_OF_STATES	(12)

#define TIME1_DEFAULT		10000
#define TIME2_DEFAULT		5000
#define TIME3_DEFAULT		10000
#define TIME4_DEFAULT		10000
#define TIME5_DEFAULT		7000
#define TIME6_DEFAULT		13000
#define TIME7_DEFAULT		10000
#define TIME8_DEFAULT		10000
#define TIME9_DEFAULT		5000
#define TIME10_DEFAULT		10000
#define TIME11_DEFAULT		5000

uint32_t timesArray[NUM_OF_TIMES];
__no_init __eeprom uint32_t eeTimesArray[NUM_OF_TIMES];
uint32_t timesArrayDefault[NUM_OF_TIMES] = {TIME1_DEFAULT,
											TIME2_DEFAULT,
											TIME3_DEFAULT,
											TIME4_DEFAULT,
											TIME5_DEFAULT,
											TIME6_DEFAULT,
											TIME7_DEFAULT,
											TIME8_DEFAULT,
											TIME9_DEFAULT,
											TIME10_DEFAULT,
											TIME11_DEFAULT};
uint32_t fullTimeInState[NUM_OF_STATES];

#define TIME1	timesArray[0]
#define TIME2	timesArray[1]
#define TIME3	timesArray[2]
#define TIME4	timesArray[3]
#define TIME5	timesArray[4]
#define TIME6	timesArray[5]
#define TIME7	timesArray[6]
#define TIME8	timesArray[7]
#define TIME9	timesArray[8]
#define TIME10	timesArray[9]
#define TIME11	timesArray[10]




stateMachine_t machine;
bool stoped = true;

void refreshFullTimeInState();

void stateIdle(bool firstTime, uint32_t timeInState);
void state1(bool firstTime, uint32_t timeInState);
void state2(bool firstTime, uint32_t timeInState);
void state3(bool firstTime, uint32_t timeInState);
void state4(bool firstTime, uint32_t timeInState);
void state5(bool firstTime, uint32_t timeInState);
void state6(bool firstTime, uint32_t timeInState);
void state7(bool firstTime, uint32_t timeInState);
void state8(bool firstTime, uint32_t timeInState);
void state9(bool firstTime, uint32_t timeInState);
void state10(bool firstTime, uint32_t timeInState);
void state11(bool firstTime, uint32_t timeInState);



void Automation_Init()
{
	StateMachine_Init(&machine, NUM_OF_STATES);
	
	StateMachine_RegisterStateFunction(&machine, state1, 1);
	StateMachine_RegisterStateFunction(&machine, state2, 2);
	StateMachine_RegisterStateFunction(&machine, state3, 3);
	StateMachine_RegisterStateFunction(&machine, state4, 4);
	StateMachine_RegisterStateFunction(&machine, state5, 5);
	StateMachine_RegisterStateFunction(&machine, state6, 6);
	StateMachine_RegisterStateFunction(&machine, state7, 7);
	StateMachine_RegisterStateFunction(&machine, state8, 8);
	StateMachine_RegisterStateFunction(&machine, state9, 9);
	StateMachine_RegisterStateFunction(&machine, state10, 10);
	StateMachine_RegisterStateFunction(&machine, state11, 11);
	StateMachine_RegisterStateFunction(&machine, stateIdle, 0);
	
	StateMachine_SetState(&machine, 1);
	
	stoped = true;
	
	int i;
	for (i=0; i < NUM_OF_TIMES; i++)	{
		timesArray[i] = eeTimesArray[i];
		
		if (timesArray[i] == 0)	{
			timesArray[i] = timesArrayDefault[i];
			eeTimesArray[i] = timesArrayDefault[i];
		}
	}
	
	refreshFullTimeInState();
}

void Automation_Process()
{
	//if (stoped == false)	{
		StateMachine_Process(&machine);
	//}
}

void Automation_Start()
{
	//stoped = false;
}

void Automation_Stop()
{
	//stoped = true;
	StateMachine_SetState(&machine, 0);
	//Обнулить все выходы
	int i;
	for (i = 0; i < 7; i++)	{
		HardwareControl_SetOutputState(i, false);
	}
}

void Automation_SetState(uint8_t newState)
{
	StateMachine_SetState(&machine, newState);
	int i;
	for (i = 0; i < 7; i++)	{
		HardwareControl_SetOutputState(i, false);
	}
}

uint32_t Automation_GetTimeout(uint8_t timeoutIndex)
{
	return timesArray[timeoutIndex];
}

void Automation_SetTimeout(uint8_t timeoutIndex, uint32_t newTimeout)
{
	//TODO Проверка индекса
	
	timesArray[timeoutIndex] = newTimeout;
	eeTimesArray[timeoutIndex] = newTimeout;
	refreshFullTimeInState();
}

void refreshFullTimeInState()
{
	fullTimeInState[0] = 0;
	fullTimeInState[1] = 0;
	fullTimeInState[2] = timesArray[0];
	fullTimeInState[3] = timesArray[1];
	fullTimeInState[4] = (timesArray[2] > timesArray[3]) ? timesArray[2]: timesArray[3];
	fullTimeInState[5] = timesArray[4];
	fullTimeInState[6] = timesArray[5];
	fullTimeInState[7] = timesArray[6];
	fullTimeInState[8] = timesArray[7];
	fullTimeInState[9] = timesArray[8];
	fullTimeInState[10] = timesArray[9];
	fullTimeInState[11] = timesArray[10];	
}

uint8_t Automation_GetCurrentState()
{
	return StateMachine_GetState(&machine);
}

uint32_t Automation_GetTimeInState()
{
	return StateMachine_GetTimeInState(&machine);
}

uint32_t Automation_GetFullTimeInState(uint8_t stateIndex)
{
	return fullTimeInState[stateIndex];	
}

void stateIdle(bool firstTime, uint32_t timeInState)
{
	
}

void state1(bool firstTime, uint32_t timeInState)
{
	if (HardwareControl_GetInputState(0) == true)	{
		Automation_SetState(2);
	}
}
									   
void state2(bool firstTime, uint32_t timeInState)
{
	if (firstTime)	{
		HardwareControl_SetOutputState(0, true);
	}
	
	if (timeInState >= TIME1)	{
		HardwareControl_SetOutputState(0, false);
		Automation_SetState(3);
	}
}

void state3(bool firstTime, uint32_t timeInState)
{
	if (timeInState >= TIME2)	{
		Automation_SetState(4);
	}
}
									   
void state4(bool firstTime, uint32_t timeInState)
{
	if (firstTime)	{
		HardwareControl_SetOutputState(1, true);
		HardwareControl_SetOutputState(2, true);
	}
	
	if (timeInState >= TIME3)	{
		HardwareControl_SetOutputState(1, false);
	}
	if (timeInState >= TIME4)	{
		HardwareControl_SetOutputState(2, false);
	}
	
	if ( (timeInState >= TIME3) && ( timeInState >= TIME4 ) )	{
		Automation_SetState(5);
	}
}
									   
void state5(bool firstTime, uint32_t timeInState)
{
	if (timeInState >= TIME5)	{
		Automation_SetState(6);
	}
}
									   
void state6(bool firstTime, uint32_t timeInState)
{
	if (firstTime)	{
		HardwareControl_SetOutputState(3, true);
	}
	
	if (timeInState >= TIME6)	{
		HardwareControl_SetOutputState(3, false);
		Automation_SetState(7);
	}	
}

void state7(bool firstTime, uint32_t timeInState)
{
	if (timeInState >= TIME7)	{
		Automation_SetState(8);
	}
}
									   
void state8(bool firstTime, uint32_t timeInState)
{
	if (firstTime)	{
		HardwareControl_SetOutputState(4, true);
	}
	
	if (timeInState >= TIME8)	{
		HardwareControl_SetOutputState(4, false);
		Automation_SetState(9);
	}		
}
									   
void state9(bool firstTime, uint32_t timeInState)
{
	if (timeInState >= TIME9)	{
		Automation_SetState(10);
	}
}
									   
void state10(bool firstTime, uint32_t timeInState)
{
	if (firstTime)	{
		HardwareControl_SetOutputState(5, true);
	}
	
	if (timeInState >= TIME10)	{
		HardwareControl_SetOutputState(5, false);
		Automation_SetState(11);
	}	
}

void state11(bool firstTime, uint32_t timeInState)
{
	if (timeInState >= TIME11)	{
		Automation_SetState(1);
	}
}