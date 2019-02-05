///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2007, Matthew Pratt
//
// Licensed under the GNU General Public License v3 or greater
//
// Date:  7 Jun 2007
///////////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include "delay.h"

static unsigned char g_last_chan = 0;

unsigned short adc_read(unsigned char adcChan)
{

    // we are changing channels then delay til the adc is ready
    if (adcChan != g_last_chan)
    {
        // 7 REFS1 0  REF table
        // 6 REFS0 1  REF table (01 means AVCC as ref)
        // 5 ADLAR 0  Right Adjust
        // 4 - 0      mux
        ADMUX = _BV(REFS0) | (adcChan & 0x1F);

        g_last_chan = adcChan;
        delay_ms(1);
    }

    ADCSRA |= _BV(ADSC); // start a conversion by writing a one to the ADSC bit (bit 6)
    while(ADCSRA & _BV(ADSC)); // wait for conversion to complete (bit 6 will change to 0)
    return ((ADCL) | ((ADCH)<<8)); // 10-bit conversion for channel 0 (PF0)
}

void adc_init(unsigned char adcChan)
{
    DDRF = 0x00; // configure a2d port (PORTF) as input so we can receive analog signals
    PORTF = 0x00; // make sure pull-up resistors are turned off (else we¡Çll just read 0xCFF)

    // 7 REFS1 0  REF table
    // 6 REFS0 1  REF table (01 means AVCC as ref)
    // 5 ADLAR 0  Right Adjust
    // 4 - 0      mux
    ADMUX = _BV(REFS0) | (adcChan & 0x1F);

    // 7 ADEN 1  ADC enable
    // 6 ADSC 0  ADC start conversion
    // 5 ADFR 0  ADC free running (0 for one shot)
    // 4 ADIF 0  ADC interupt flag
    // 3 ADIE 0  ADC interupt enable
    // 2-0  ADC prescaler
    ADCSRA = _BV(ADEN) | 0x6; // selects div by 64 clock prescaler
}
