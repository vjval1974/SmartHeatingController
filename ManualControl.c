//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 15 Feb 2019
// Email Address: W0085400@gmail.com
// 
// Purpose:
//
//
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// Included Libraries
//-------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdint.h>
#include "menues.h"
//#include "main.h"
#include "menu.h"

#include "keypad.h"
#include "hd44780.h"
#include "hd44780_settings.h"

#include "delay.h"

#include <string.h>
#include "pwm.h"

volatile uint8_t power = 0;

void ManualControl_applet()
{
    printlcd(1, 0, "MANUAL CONTROL");
}


//     //RTC_displayTime(1, 6); //display the time on the LCD
        //     char line1[TOTALCELLS];
        //     sprintf(line1, "[%s] %s", GetCurrentAppletName(), RTC_GetTimeString());
        //     SetLineIn(2, &MainApplet, "Output %02d%% ", pwm_get_A_pct());
        //     char row[TOTALCELLS] = "                ";
        //     pctToBar(pwm_get_A_pct(), row);
        //     SetLineIn(4, &MainApplet, row);
            
        //     SetLine(1, line1); // sets line in currently active applet
            
        //     Display();
        //     PollProgram(&program); // if there is one to run

        //     timeLast = timeNow;



uint8_t ShouldUpdate()
{
    static uint8_t last_power = 255; 
    if (last_power != power)
    {
        last_power = power;
        return 1;
    }
    else return 0;
}

static char row[LCD_DISPLAY_CHARS] = "                    ";

void UpdateDisplay()
{
    printlcd(1, 0, "MANUAL CONTROL");
    
    pctToBar(pwm_get_A_pct(), row);
    printlcd(4, 0, row);  
    
}

void ManualControl_key(unsigned char key)
{
    //up arrow, increase power
    if (key == BUTTON_UP) 
    {
        power += 10;
        if (power > 100)
            power = 100; 
    }
    
    //down arrow, move indicator down
    else if (key == BUTTON_DOWN)
    {
        power -= 10;
        if (power == 0 || power >= 100) // because we are using a unsigned int. 
            power = 0; 
    }
    
    //left arrow. go back one level in depth. 
    else if (key == BUTTON_LEFT)
    {
        power = 0;
        pwm_set_A(OFF, power);    
        //menu_return_from_applet();
    }
    
    //right arrow. step forward one level in depth
    else if (key == BUTTON_RIGHT)
    {
              
       
        
    }
    if (ShouldUpdate() == 1)
    {
        //printf("Power = %u\r\n", power);
        pwm_set_A(ON, power);    
        UpdateDisplay();
        
    }
 

}