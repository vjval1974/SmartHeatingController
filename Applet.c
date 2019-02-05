//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 19th Jan 2019
//
// Purpose:
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Included Libraries
//-------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include "util.h"
#include "hd44780.h"
#include "delay.h"
#include "RTC_routines.h"
#include "i2c_routines.h"
#include "keypad.h"
#include "Interrupts.h"
#include "timercounter3.h"
#include "timer.h"
#include "adc.h"
#include "spi.h"
#include <math.h>
#include <limits.h>
#include "timercounter2.h"
#include "pwm.h"
#include "Applet.h"


struct Applet {
    char *Name;
    char *Line1;
    char *Line2;
    char *Line3;
    char *Line4;
};


Applet  ** applet; 
void InitApplet(Applet ** ap)
{
    *ap = (Applet*)calloc(1, sizeof(Applet));
    (*ap)->Line1 = calloc(20, sizeof(char));
    (*ap)->Line2 = calloc(20, sizeof(char));
    (*ap)->Line3 = calloc(20, sizeof(char));
    (*ap)->Line4 = calloc(20, sizeof(char));
    (*ap)->Name = calloc(20, sizeof(char));
}

void SetName(const char * name, Applet ** ap)
{
    sprintf((*ap)->Name, name);
}

char * GetCurrentAppletName()
{
    return (*applet)->Name;
}

void SetApplet(Applet ** newApplet)
{
   //printf("NewApplet is at %x\r\n", newApplet );
    
    // if (*applet)
    //     ForgetApplet(*applet);
    //*newApplet = (Applet*)calloc(1, sizeof(Applet));
    applet = newApplet;
    //("applet is at %x\r\n", applet);
    lcd_clrscr();
   // printlcd(2, 0, "Na=%x, &Na=%x", *newApplet, newApplet);
   // printlcd(3, 0, "Aa=%x, &Aa=%x", *applet, applet);

    //printlcd(3, 0, "l1=%x" (*applet)->Line1
    
}
void ForgetApplet(Applet * appletToForget)
{
       //free(appletToForget);
}

void Display()
{
    lcd_clrscr();
    if (*applet && applet)
    {
        if ((*applet)->Line1 != NULL)
            printlcd(1, 0, (*applet)->Line1);
        if ((*applet)->Line2 != NULL)
            printlcd(2, 0, (*applet)->Line2);
        if ((*applet)->Line3 != NULL)
            printlcd(3, 0, (*applet)->Line3);
        if ((*applet)->Line4 != NULL)
            printlcd(4, 0, (*applet)->Line4);
    }
}

void SetLine(int num, char *szFormat, ...)
{
    char szBuffer[20]; //in this buffer we form the message
    int NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
    va_list pArgs;
    va_start(pArgs, szFormat);
    vsnprintf(szBuffer, NUMCHARS -1, szFormat, pArgs);
    va_end(pArgs);
    //printf(szBuffer);
    switch(num)
    {
        case 1:
        {
            sprintf((*applet)->Line1, szBuffer);
            break; 
        }
        case 2: 
        {
            sprintf((*applet)->Line2, szBuffer);
            break;
        }
        case 3: 
        {
            sprintf((*applet)->Line3, szBuffer);
            break;
        }
        case 4: 
        {
            sprintf((*applet)->Line4, szBuffer);
            break;
        }
        default:
        {
            return;
        }
    }

}


    

void SetLineIn(int num,  Applet ** ap, char *szFormat, ...)
{
  //  printf("Setting line in applet at  %x\r\n", ap);
 
    char szBuffer[20]; //in this buffer we form the message
    int NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
    va_list pArgs;
    va_start(pArgs, szFormat);
    vsnprintf(szBuffer, NUMCHARS -1, szFormat, pArgs);
    va_end(pArgs);
   // printf(szBuffer);
    switch(num)
    {
        case 1:
        {
            sprintf((*ap)->Line1, szBuffer);
            break; 
        }
        case 2: 
        {
            sprintf((*ap)->Line2, szBuffer);
            break;
        }
        case 3: 
        {
            sprintf((*ap)->Line3, szBuffer);
            break;
        }
        case 4: 
        {
            sprintf((*ap)->Line4, szBuffer);
            break;
        }
        default:
        {
            return;
        }
    }

}
