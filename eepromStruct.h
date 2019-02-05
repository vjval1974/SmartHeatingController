// Copyright (C) 2019, Brad Goold, All Rights Reserved.
//
// Authors: Brad Goold
//
// Date: 26 January 2019
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EEPROMSTRUCT_H
#define EEPROMSTRUCT_H
#include <stdint.h>
#include "Program.h"

void UploadProgramToEeprom(Program * program);
void UploadProgramsToEeprom(Program * programs[4]);
void LoadProgramsFromEeprom(Program * programs[4]);
void init( void(*pwmSetterFunc)(char state, char pct), 	void(*pollingFunc)());

#endif