#include "StateMachine.h"
#include <stdlib.h>

#include "UptimeService.h"

void StateMachine_Init(stateMachine_t *this, uint8_t numOfStates)
{
	this->numOfStates = numOfStates;

	this->stateFuncArray = malloc(sizeof(ptrStateFunc) * numOfStates);
	
	int i;
	for (i=0; i < numOfStates; i++)	{
		this->stateFuncArray[i] = NULL;
	}
}

void StateMachine_RegisterStateFunction(stateMachine_t *this, ptrStateFunc func, uint8_t stateIndex)
{
	//TODO проверка stateIndex
	
	this->stateFuncArray[stateIndex] = func;
}

void StateMachine_Process(stateMachine_t *this)
{
	bool firstTime = false;

	if (this->prevState != this->currentState)	{
		this->prevState = this->currentState;
		
		this->time = UptimeService_GetUptime();
		firstTime = true;
	}
	uint32_t timeInState = UptimeService_GetTimeDiff(this->time);
	
	this->stateFuncArray[this->currentState](firstTime, timeInState);
	
}

void StateMachine_SetState(stateMachine_t *this, uint8_t newState)
{
	this->currentState = newState;
}

uint8_t StateMachine_GetState(stateMachine_t *this)
{
	return this->currentState;
}

uint32_t StateMachine_GetTimeInState(stateMachine_t *this)
{
	return UptimeService_GetTimeDiff(this->time);
}