//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 19 Mar 2009
// Email Address: W0085400@mail.connect.usq.edu.au
// 
// Purpose:
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// Included Libraries
//-------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include "util.h"

#include "delay.h"

//-------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------

// KEYS_GET 
//-------------------------------------------------------------------------
/* 
This is some basic source code for a 4x3 matrix keypad
The basic functionality of the code is this:
   1. Set one column in the matrix high. 
   2. Read the corresponding row. 
   3. If we have a value
       3a. check it for a debounce
       3b. then leave the loop. 
   4. increment to the next column
   5. go around again. 
This code can be improved in many ways, one being to wait until all columns 
are scanned, then check conflicting values (ie if two buttons have been 
pressed at once or one button is held down). 
*/
//-------------------------------------------------------------------------
unsigned char keys_get(void)
{
    unsigned char debounce = 0, inputvar = 0, ii = 0;
    static unsigned char index = 1, lastkey;
    unsigned char mask = 0x07, key = 0xFF;
    char keypad[3][4];
    char retval = 0xff;
    
//Keypad Map
//---------------------------------
    keypad[0][0] = 0x0B;
    keypad[0][1] = 0x09;
    keypad[0][2] = 0x06;
    keypad[0][3] = 0x03;
    keypad[1][0] = 0x00;
    keypad[1][1] = 0x08;
    keypad[1][2] = 0x05;
    keypad[1][3] = 0x02;
    keypad[2][0] = 0x0a;
    keypad[2][1] = 0x07;
    keypad[2][2] = 0x04;
    keypad[2][3] = 0x01;    
   

//----------------------------------

    // scan keypad
    for (index = 1; index <= 4; index = index * 2)
    {
        DDRA = 0xFF;             // set to outputs
        PORTA = 0x07 & index;    // set output on single column
        delay_us(2);            // wait for pullups to set
        DDRA = 0x07;             // now set to inputs
        inputvar = PINA;        // read the input lines corresponding
        inputvar = (inputvar & ~mask) >> 3;  // shift right for byte value
        
        // Check for input bounce
        if (inputvar != 0)
        {
            delay_us(150);
            debounce = (PINA & ~mask) >> 3;
            if (debounce == inputvar) //input is true
            {
                if (debounce == 4) debounce = 3;
                if (debounce == 8) debounce = 4;
                key = keypad[ii][debounce-1]; //get value from array
            }
            else //input was false.
            {
                debounce = 0; 
                key = 0xFF;
            }
        }
        if (key != 0xFF) break; // if we have a key pressed, lets leave the loop
        
        //increment array index
        ii++;
        if (ii > 2) ii = 0;
        delay_us(5);
    }
    
  
    if (lastkey ^ key)
    {
        if (key != 0xFF)
        {
            delay_us(10000);
            retval =  key;
            
        }
        else retval = 0xFF;
    }
    lastkey = key;
   
    return retval;
   
}
   
    
    
