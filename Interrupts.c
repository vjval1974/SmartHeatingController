//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 29 Oct 2009
// Email Address: W0085400@mail.connect.usq.edu.au
// 
// Purpose: To handle the interrupts on the Atmega 128
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// Included Libraries
//-------------------------------------------------------------------------
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "util.h"
#include "hd44780.h"
#include "UART_routines.h"
#include "Interrupts.h" 

/*
External Interrupts use the Registers : EICRA, EICRB, and EIMSK
---------------------------------------------------------------

To set up an interrupt, you must set the appropriate registers
in the initialisation function. After that you can write the 
code in the ISR routines (Interrupt Service Routines). 
Remember that the Global Interrupt Enable bit in the Status 
Register must be set to 1 for the global interrupts to work. 
To set the global interrupt enable, you use the sei() function 
within the initialisation function. 

SETTING UP THE REGISTERS TO ENABLE A PARTICULAR REGISTER:
---------------------------------------------------------

EICRA and EICRB are the control registers for External interrupts
0 - 7. You must set the appropriate bits in order to handle the 
interrupt in the desired way. 
EICRA bits 0 and 1 are for setting the control on Interrupt 0. 
EICRA bits 2 and 3 are for setting the control on Interrupt 2.
and so on. 
The configuration of the bits are as follows:
Set both bits to 0: the interrupt will always trigger if the
state of the corresponding pin is zero. This is good for a power
failure checking on a particular device etc. 
Set bit 0  to value 0 and bit 1 to value 1: this triggers the 
interrupt on the falling edge of the corresponding pin. 
Set bit 0 to value 1 and bit 1 to value 1: this triggers the 
interrupt on the rising edge of the corresponding pin. 

EIMSK is the masking register which enables the particular 
interrupt. Set the bits in this register to 1 to enable each 
interrupt required. 

EXAMPLE CODE : Port D pin 3 to trigger interrupt on falling edge

void init_interrupts(void)
{
    cli(); // clear the global interrupt enable bit in case its already 
           // enabled so that an interrupt doesnt get called during setup

    EICRA = _BV(ISC21)| _BV(ISC20); //sets interrupt 2 (PORT D pin 3)
                                    //for triggering on rising edge

    EIMSK = _BV(INT2);  //enables the external interrupt 2 (PD pin 3)
    
    sei(); //sets the global interrupt flag in the status register 
           //back to 1 so that interrupts are enabled again. 
}




Writing the ISR routine:
When an interrupt is triggered, the global interrupt enable bit
is set to zero, this disables any other interrupt from triggering
from inside the routine. You can set this to 1 in the software by
using sei() within your interrupt routine. This means that another 
interrupt with higher priority will be called. Priority is denoted
by the interrupt number whereby 0 is the highest and 7 is the lowest. 
The values in the Status register are NOT saved when the ISR is 
entered. It is good practice to save the status register in a dummy
byte upon entering the ISR. 
The function prototype called ISR(vector) is used to locate the function
to the correct Interrupt vector in program memory. INT0_vect is the 
location where interrupt 0 is located in the program memory.

IMPORTANT NOTE: Make sure you use create an ISR for the correct enabled
    interrupt. If no ISR is created, your avr will revert to the reset
    vector by default, resetting the avr on interrupt


EXAMPLE CODE: ISR routine for pin 3 on Port D

ISR(INT2_vect)
{
    char sreg = SREG; //save the status register upon entering routine

    // you can put in the instruction : sei(); to enable interrupts
    // during this routine. This will enable higher priority interrupts
    // to break this routine to execute another, then return. 

    
    //place interrupt code here

    SREG = sreg; 
}
*/
char rx_buf[100];
char rx_buf1[100];
int ii;
// ------------------------------------------------------------------------
//*******FUNCTIONS***********
//-------------------------------------------------------------------------
void init_interrupts(void)
{
    cli(); // clear the global interrupt enable bit in case its already 
           // enabled so that an interrupt doesnt get called during setup

    EICRA = _BV(ISC21)| _BV(ISC20); //sets interrupt 2 (PORT D pin 3)
                                    //for triggering on rising edge

    EIMSK = _BV(INT2); //enables the external interrupt 2 (PD pin 3)
    for (ii = 0; ii < 100; ii++)
    {
        rx_buf[ii] = 0;
    }

    sei(); //sets the global interrupt flag in the status register 
           //back to 1 so that interrupts are enabled again. 
}


ISR(INT2_vect)
{
    char sreg = SREG; //save the status register upon entering routine

    // you can put in the instruction : sei(); to enable interrupts
    // during this routine. This will enable higher priority interrupts
    // to break this routine to execute another, then return. 

    
    //place interrupt code here

    SREG = sreg; 
}
/*
ISR(USART0_RX_vect)
{
    char sreg = SREG;
    cli();
    
    char xx = receiveByte();
    
    switch (xx)
    {
    case  '\n' :
    {
        for (ii = 0; ii < 100; ii++)
        {
            rx_buf[ii] = 0;
        }
    }
    break;
    
    case  '\r':
    {
        for (ii = 0; ii < 100; ii++)
        {
            rx_buf[ii] = 0;
        }
    }
    break;
    
    default: 
        
        if (xx != 0 )
        {
            for (ii = 99; ii>=1; ii--)
            {
                rx_buf[ii] = rx_buf[ii-1];
            }
            rx_buf[0] = xx;
            printlcd(1, 0, "%s", rx_buf);   
        }
    }
    
    sreg = SREG;
    sei();
}

ISR(USART1_RX_vect)
{
    char sreg = SREG;
    cli();
    
    char xx = receiveByte1();
    
    switch (xx)
    {
    case  '\n' :
    {
        for (ii = 0; ii < 100; ii++)
        {
            rx_buf1[ii] = 0;
        }
    }
    break;
    
    case  '\r':
    {
        for (ii = 0; ii < 100; ii++)
        {
            rx_buf1[ii] = 0;
        }
    }
    break;
    
    default: 
        
        if (xx != 0 )
        {
            for (ii = 99; ii>=1; ii--)
            {
                rx_buf1[ii] = rx_buf1[ii-1];
            }
            rx_buf1[0] = xx;
            printlcd(1, 0, "%s", rx_buf1);   
        }
    }
    
    sreg = SREG;
    sei();
}
*/

        
