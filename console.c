#include <avr/eeprom.h>
#include <stdio.h>
#include <stdlib.h>
#include "delay.h"
#include "console.h"
#include "UART_routines.h"



char line[32]; 
static void clr_line(void)
{
    
    for(int ii = 0; ii < 32; ii++)
    {
        line[ii] = 0;
    }
    
}

uint8_t getConsoleKey(void)
{

    uint8_t key = 255;
    if (uart_get_last_line(line) == 1)
    
    {
        key = strtol(line, NULL, 10);
        clr_line();
        
    }
     return key;
    
}

