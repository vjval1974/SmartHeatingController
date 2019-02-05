// Copyright (C) 2019, Brad Goold, All Rights Reserved.
//
// Authors: Brad Goold
//
// Date: 5 February 2019
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROGRAM_H
#define PROGRAM_H
#include <stdint.h>


typedef struct
{
    uint8_t Id;
    uint8_t programId;
    uint8_t PwmPct;
    uint16_t TimeInSeconds;
    char Name[20];
    uint16_t ElapsedSeconds;	
} ProgramStep;

typedef struct
{
    uint8_t Id;
    char Name[20];
    ProgramStep Steps[5];
    void(*pwmSetterFunc)(char state, char pct);
	void(*pollingFunc)();
    uint8_t CurrentStep;
} Program;


	
extern void (*g_key_handler)(uint8_t key);
extern Program * programs[4]; 
void printStep(ProgramStep * step);
void printProgram(Program * program);

void test();
void mainKey(uint8_t key);
void programSelectionDisplay(uint8_t key);




#endif