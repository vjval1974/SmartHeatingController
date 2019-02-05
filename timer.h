///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2007, Matthew Pratt
//
// Licensed under the GNU General Public License v3 or greater
//
// Date: 21 Jun 2007
///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
void timer_init(void);
uint32_t timer_get(void);

#define SECONDS_IN_TICKS(secs)    ((secs) * 244)
#define MINS_IN_TICKS(mins)       ((mins) * 14648)
#define SECONDS_FROM_TICKS(ticks) ((ticks) / 244)
#define MINS_FROM_TICKS(ticks)    ((ticks) / 14648)
#define _HALF_SECONDS_IN_TICKS(secs) ((secs) * 122)
#define _THIRD_SECONDS_IN_TICKS(secs) ((secs) * 61)  
//#define RUN_PERIOD(ms) {static uint32_t last = 0; 
//uint32_t now = timer_get(); 
//if (now - last > MS_IN_TICKS(ms)) {last = now;} else return;


#endif
