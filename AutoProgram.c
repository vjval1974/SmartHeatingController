//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 11th Jan 2019
//
// Purpose:
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Included Libraries
//-------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include "util.h"
#include "hd44780.h"
#include "delay.h"
#include "RTC_routines.h"


#include "keypad.h"
#include "Interrupts.h"
#include "timer.h"
#include "adc.h"
#include <math.h>
#include <limits.h>
#include "pwm.h"
#include "AutoProgram.h"
#include "Applet.h"
#include "Program.h"

#define EndStepName "End"

Program PROGMEM const NullProgram = 
{
    0, 
    (void *)NULL,
        
    {
        (void *)NULL
    },
    (void *)NULL, 
    (void *)NULL,
    0,
};

ProgramStep PROGMEM const EndStep = { 0,    0,      0,     0,     {EndStepName},     0 };



void (*stepPollingFunc)(Program *program);
void NextStep(Program *program);
void StepPollingFunc(Program *program);
void InitialiseStep(Program *program);
uint16_t numSteps(Program *program);
void Finished(Program *program);

void SetUp(Program *program)
{
	if (strcmp(program->Name, "NOPROGRAM") != 0)
	{
		program->CurrentStep = 0;
		
		InitialiseStep(program);
		stepPollingFunc = StepPollingFunc;
	}
}

void PollProgram(Program *program)
{
	if (stepPollingFunc != NULL)
		stepPollingFunc(program);
}

void InitialiseStep(Program *program)
{
	SetLine(3, program->Steps[program->CurrentStep].Name);
	program->Steps[program->CurrentStep].ElapsedSeconds = 0;

	if (program->pwmSetterFunc != NULL)
		program->pwmSetterFunc(ON, program->Steps[program->CurrentStep].PwmPct);
	
}

void RunStep(Program *program)
{
	if (program->Steps[program->CurrentStep].ElapsedSeconds > program->Steps[program->CurrentStep].TimeInSeconds)
	{
		NextStep(program);
	}

	SetLine(2, "S:%2d, Elapsed=%2d,", program->CurrentStep, program->Steps[program->CurrentStep].ElapsedSeconds);
}

void SetElapsedTime(ProgramStep *step)
{
	//uint16_t currentSeconds = RTC_get_seconds();
	step->ElapsedSeconds++;
}

void StepPollingFunc(Program *program)
{
	SetElapsedTime(&program->Steps[program->CurrentStep]);
	RunStep(program);
}

void NextStep(Program *program)
{
	program->CurrentStep++;

	if (program->CurrentStep >= numSteps(program))
	{
		Finished(program);
		return;
	}

	InitialiseStep(program);
}

void Finished(Program *program)
{
	stepPollingFunc = NULL;
	program->pwmSetterFunc(OFF, 0);
}

uint16_t numSteps(Program *program)
{
	if (program->Steps != NULL)
	{
		uint8_t i = 0;
		while (program->Steps[i].Name != NULL)
			i++;
		return i - 1;
	}
	return 0;
}
