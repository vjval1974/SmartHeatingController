#ifndef _AutoProgram_h_
#define _AutoProgram_h_

#include "Program.h"
#include <avr/pgmspace.h>
// struct ProgramStep { 
// 	uint16_t TimeInSeconds;
// 	uint16_t ElapsedSeconds;	
// 	uint8_t PwmPct;
// 	char Name[20];
// };
// typedef struct ProgramStep aProgramStep;

// struct Program
// {
// 	uint8_t CurrentStep;
// 	void(*pwmSetterFunc)(char state, char pct);
// 	void(*pollingFunc)();
// 	aProgramStep Steps[5];
// 	char Name[20];
// };
// typedef struct Program aProgram;

// extern aProgramStep PROGMEM const EndStep;


void SetUp(Program *program);
void PollProgram(Program* program);
extern Program PROGMEM const NullProgram; 
extern void (*stepPollingFunc)(Program* program);


#endif
