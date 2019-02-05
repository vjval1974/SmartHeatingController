///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019, Brad Goold, All Rights Reserved.
//
// Authors: Brad Goold
//
// Date: 26 January 2019
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MCUCSR_H
#define MCUCSR_H
#include <stdint.h>

// supply your printf-like function and Ill use it to display the text
void displayLastResetCause( int (*func)(const char *szFormat, ...) );


#endif