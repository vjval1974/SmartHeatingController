#include <avr/io.h> 
#include <avr/interrupt.h>
#include <stdint.h>
#include "mcucsr.h"
#include <avr/wdt.h>
#include <avr/common.h>
#include <stdio.h> 

//---------------------------------------------------
// Get MCUCSR (MCU Control Status Register) value
// and store it in an uninitialised section of memory
//---------------------------------------------------
#ifndef MCUCSR
#define MCUCSR    _SFR_IO8(0x34) 
#endif
uint8_t mcucsr_mirror __attribute__((section (".noinit"))); 
void get_mcucsr() __attribute__((naked))  __attribute__((section (".init3")));

void get_mcucsr(void) {
    mcucsr_mirror = MCUCSR;
    MCUCSR = 0;
    wdt_disable(); // disables the watchdog just incase
    //now do some code to initialise outputs for safety
}

void displayLastResetCause( int (*func)(const char *szFormat, ...) )
{
    func("Last Reset Caused By:\r\n");
    switch (mcucsr_mirror)
    {
    case 1:
        func("Normal Power-Down\r\n");
        break;
    case 2:
        func("External Reset\r\n");
        break;
    case 4:
        func("BrownOut Detection\r\n");
        break;
    case 8:
        func("WatchDog Timer\r\n");
        break;
    case 16:
        func("JTAG Interface\r\n");
        break;
    default: 
        func("Unknown\r\n");
        break;
    }
}

    
