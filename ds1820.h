///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2007, Matthew Pratt
//
// Licensed under the GNU General Public License v3 or greater
//
// Date:  2 Jun 2007
///////////////////////////////////////////////////////////////////////////////

#ifndef DS1820_H
#define DS1820_H

#include <inttypes.h>

uint8_t ds1820_search(void);

void ds1820_process(void);
int ds1820_get_temp(uint8_t index, unsigned char *temp);



#endif
