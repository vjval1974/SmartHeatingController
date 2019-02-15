#include <avr/io.h> 
#include <avr/interrupt.h>
#include <stdint.h>
#include "timer.h"

volatile uint32_t g_ticks = 0;

ISR(TIMER1_OVF_vect)
{
	g_ticks++;
}

void timer_init(void)
{
ASSR = 0;
TCCR1A = 0;
TCCR1B = 0x01; 
TIMSK |= _BV(TOIE1);
TCNT1 = 0;
}



uint32_t timer_get(void)
{
return g_ticks;
}
