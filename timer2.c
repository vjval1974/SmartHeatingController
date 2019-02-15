//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 18 Dec 2010
// Email Address: W0085400@umail.usq.edu.au
// 
// Purpose:
// Pre:
// Post:
// RCS $Date: 2012/04/03 04:04:00 $
// RCS $Revision: 1.2 $
// RCS $Source: /home/brad/Documents/AVR/atmega128_flow/RCS/timer2.c,v $
// RCS $Log: timer2.c,v $
// RCS Revision 1.2  2012/04/03 04:04:00  brad
// RCS Cleaned up a little
// RCS
// RCS Revision 1.1  2012/04/02 22:42:25  brad
// RCS Initial revision
// RCS
// RCS Revision 1.1  2010/12/29 00:42:17  brad
// RCS Initial revision
// RCS
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// Included Libraries
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "timer2.h"

#include <avr/pgmspace.h>
#include "util.h"
#include "keypad.h"

#define T2_PORT D
#define T2_PIN_MASK 0x80;  //port D7

//
//-------------------------------------------------------------------------
volatile uint8_t final_compare_val = 0;
volatile char final_compare_enable = 0;
volatile uint32_t t2_overflows = 0;
volatile uint32_t t2_wraps = 0;
void timer2_init(void){

    //set the T2 pin to an input
    //and set the pullup resistor on. (we are looking for a 
    //common collector input (drops to 0 on pulse)
    // DDR(T2_PORT) &= ~T2_PIN_MASK;
    // PORT(T2_PORT) |=  T2_PIN_MASK;
    
    // Clock at 1024 Prescale
    TCCR2 |= _BV(CS20);  
    TCCR2 |= _BV(CS22);

    TCCR2 |= _BV(WGM21); // set up for CTC (clear counter on match) 
    
    
    OCR2 = 200; //each overflow will occur at 200 pulses. Counter
                //will be cleared. 
    // each overflow may be called at abot 860ml with a flow 
    // rate of 15-20l/min

    // enable timer overflow interrupts to count the number of 
    // overflows that occur 
    TIMSK |= _BV(OCIE2);

    //init values 
    t2_overflows = 0;
    TCNT2 = 0; //

} 

ISR(TIMER2_OVF_vect){ // called each 200 pulses
  
     unsigned char sreg;
    
    // save the global interrupt flag
     sreg = SREG;
    
    // disable interrupts
      cli();

    // update the number of overflows
  
    t2_overflows++; 
   
    // Restore the global interrupt flag
     sei();

    SREG = sreg;
   
    
}

uint32_t timer2_get(void){
    return (t2_wraps * 200) + TCNT2;
}

void timer2_reset(void){
    t2_wraps = 0;
    TCNT2=0;
}

volatile uint8_t shouldGetKeys = 0;

unsigned char ShouldGetKeys()
{
    return shouldGetKeys;
}

unsigned char getKey()
{
    unsigned char key = 0xff;
    if (shouldGetKeys == 1)
        key = keys_get();

    shouldGetKeys = 0;
    return key;
}

ISR(TIMER2_COMP_vect){
 unsigned char sreg;
    
    // save the global interrupt flag
     sreg = SREG;
    
    // disable interrupts
      cli();

    // update the number of overflows
  
    t2_wraps++; 
    shouldGetKeys = 1;
    // printf("timer wrap %u\r\n", t2_wraps);     
 // printf_P(PSTR("\ntccr2 = %x\n"), TCCR2);  
    // Restore the global interrupt flag
     sei();

    SREG = sreg;
}
