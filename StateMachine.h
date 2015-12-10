#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef void (*ptrStateFunc)(bool firstTime, uint32_t timeInStates);

typedef struct	{
	uint8_t currentState;
	uint8_t prevState;
	ptrStateFunc *stateFuncArray;
	uint32_t time;
	uint8_t numOfStates;
} stateMachine_t;

void StateMachine_Init(stateMachine_t *this, uint8_t numOfState);
void StateMachine_RegisterStateFunction(stateMachine_t *this, ptrStateFunc func, uint8_t stateIndex);
void StateMachine_Process(stateMachine_t *this);
void StateMachine_SetState(stateMachine_t *this, uint8_t newState);
uint8_t StateMachine_GetState(stateMachine_t *this);
uint32_t StateMachine_GetTimeInState(stateMachine_t *this);