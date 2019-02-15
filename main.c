//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 6 Feb 2019
// Email Address: W0085400@mail.gmail.com
//
// Purpose: Smart heating controller of electric frying pan that is 
//          currently switched via a very bad simmerstat. PWM out goes
//          to SSR connected to 240v socket. 
//-------------------------------------------------------------------------
#ifndef __AVR_ATmega128__
#define __AVR_ATmega128__
#endif

#ifndef __ASSERT_USE_STDERR
#define __ASSERT_USE_STDERR
#endif


// #if defined __AVR_ATmega128__
// #include <avr/iom128.h>
// #endif 

#ifndef F_OSC
#define F_OSC 16000000
#endif
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
#include "UART_routines.h"
#include "util.h"
#include "hd44780.h"
#include "delay.h"
#include "RTC_routines.h"
#include "keypad.h"
#include "Interrupts.h"
#include "timer.h"
#include "timer2.h"
#include "adc.h"
#include <math.h>
#include <limits.h>
#include "pwm.h"
#include "AutoProgram.h"
#include "Applet.h"
#include "mcucsr.h"
#include "eepromStruct.h"
#include "console.h"
#include "i2c_routines.h"
#include "menu.h"
#include "ManualControl.h"


/*

Pins used (known):

PORT A: all pins for keypad.
PORT B: 1 - 4 will be used for the SPI. 5-8 are available, but are PWM also
PORT C: all pins for lcd data
PORT D:
        pins 1 and 2 are used for the I2C bus
        pins 3 and 4 are either serial port 1 rxd and txd or interrupts 2 and 3
        pins 5 and 6
PORT E: pins 1 ,2 and 3 are used for Usart 0
        pins 4 ,5 amd 6 can be interrupts or PWM
        pin 7 can be external interrupt 6 or counter 3 clock input
        pin 8 IS used for the external interrupt on timer 3 
PORT F: all pins are configurable for ADC's

PORT G: Used for LCD

*/
//---------
//Constants
//---------
const int PWM_FREQ = 7; //Hz
const int PWM_PE3_STATE = ON;
const int PWM_PE4_STATE = OFF;
const int PWM_PE5_STATE = OFF;


// Program program = 
// {
//     0, 
//     "Program 1",
        
//     {
//         //ID    ProgID  PwmPct  Time    Name        Elapsed
//         { 1,    1,      10,     1,     "Step1",     0 },
//         { 2,    1,      20,     2,     "Step2",     0 },
//         { 3,    1,      30,     5,     "Step3",     0 },
//         { 4,    1,      40,     8,     "Step4",     0 },
//         { 5,    1,      50,     2,     "Step5",     0 }
//     },
//     pwm_set_A, 
//     PollProgram,  
//     0,
// };

struct menu diag_menu[] =
{
//  (Text)       (*next)          (*func)              (*key handler)
    {"??",     NULL,           NULL,                NULL},
    {"??",       NULL,           NULL,                NULL},
    {"??",       NULL,           NULL,                NULL},
    {"??",       NULL,           NULL,                NULL},
    {NULL,          NULL,           NULL,                NULL}
    
};





struct menu main_menu[] =
{
//  (Text)       (*next)          (*func)              (*key handler)
    {"Setup",     NULL,           NULL,                NULL},
    {"Manual",       NULL,           UpdateDisplay,                ManualControl_key},
    {"Auto",       NULL,           NULL,                NULL},
    {"Diag",       diag_menu,           NULL,                NULL},
    {NULL,          NULL,           NULL,                NULL}
    
};


    
//----------
//Prototypes
//----------
static char shouldUpdateTime();



// Applets
Applet *MainApplet;
const char *MainAppletName = "Main";
Applet *AutoApplet;
const char *AutoAppletName = "Auto";

void main_key(char key);

//==================================================================
//===================
//FUNCTION PROTOTYPES
//===================
//--------------------------------------
// Display message upon return from main
//--------------------------------------
void exit_message()
    __attribute__((naked))
    __attribute__((section(".fini8")));
//-------------------------------------------------------

static void (*g_poll_func)(void);

void set_poll_func(void (*func)(void))
{
    g_poll_func = func;
}


Program program;
//==================================================================
//------------
//Main Program
//------------
int main(void)
{
    //------------------------------------------------------
    // INITIALISATION
    //------------------------------------------------------

    //Set up data direction registers and pullups
    DDRG = 0xFF;  //port G is outputs
    PORTG = 0x00; //port G has been pulled low.
    DDRC = 0x00;
    PORTC = 0x00;
    DDRA = 0x00;
    PORTA = 0x00;
    DDRD = 0x00;
    DDRB = 0x80;

    uart1_init();
    
    delay_ms(50);
    printf("USART OK...\r\n");
    delay_ms(50);

    //Initialisation of peripherals
   
      
    init_interrupts();
    printf("Interrupts configured...\r\n");
    timer_init();
    timer2_init();
    printf("Basic timer configured...\r\n");
    adc_init(ADC_CHAN_0);
    printf("ADC channel 0 configured...\r\n");
    twi_init();
    if (shouldUpdateTime())
    {
        RTC_updateTime();
        RTC_updateDate();
    }
    
    printf("I2C comms configured...\r\n");
    printf("trying RTC...\r\n");
    RTC_displayDate();
    RTC_displayTime();
    RTC_displayDay();
    fflush(stdout);
    displayLastResetCause(printf);
    
   // printlcd(1, 1, "Initializing...");
    program = NullProgram;
    g_poll_func = NULL;

    init(pwm_set_A, PollProgram);

    lcd_init(); //HD44780U LCD
    #ifndef DONT_USE_LCD
    printlcd(1, 1, "LCD OK");
    printf("LCD OK...\r\n");
    delay_ms(500);    
    lcd_clrscr();
    menu_set_root(main_menu);  
    
    //sprintf(diag_menu[0].display_text, "ASDF");

    SetMenuNames(&diag_menu);
    #endif

    g_key_handler = menu_key;

    
    init_pwm(PWM_FREQ, PWM_PE3_STATE, PWM_PE4_STATE, PWM_PE5_STATE);
    programSelectionDisplay(0);
    sei();
    //GetProgramsAndTranslate(programs);
   
    // InitApplet(&MainApplet);
    // InitApplet(&AutoApplet);
    // SetName(MainAppletName, &MainApplet);
    // SetName(AutoAppletName, &AutoApplet);
    // SetApplet(&MainApplet);

    


    //-------------------
    // Set WatchDog Timer
    //-------------------
    //   wdt_enable(WDTO_60MS);
    //--------------------------------------------------------
    // Main Program Cycle
    //--------------------------------------------------------

    while (1)
    {
        //-----------
        //handle keys
        //-----------
        //uint8_t key = getKey(); // use this when taking keys from USART
        
        uint8_t key = getKey(); // timer 2 overflows and allows keys to be retrieved 7 times a second. 


        // uint8_t key = keys_get();

        if (key != 255)
        {
            //printf("Keyreturned = %d\r\n", key);

            g_key_handler(key);
        }

        // delay_ms(200); // really this should be driven by an interrupt on a keypress

        if (g_poll_func)
            g_poll_func();

        // TEST CODE 
        // uint16_t key_now = timer_get();
        // static uint16_t key_last;

        // if (key_now - key_last > SECONDS_IN_TICKS(1) / 100)
        // {
        //     key_last = key_now;
        //     char key = keys_get(); // check if a key has been pressed.
        //     g_key_handler(key);
        // }

        // timeNow = RTC_get_seconds();
        // if (timeNow - timeLast >= 1)
        // {

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
        // }
      

        wdt_reset(); //Clear the WatchDog timer flag
    }
    //----------------------------------------------------------
    //End of Main Program Cycle
    //----------------------------------------------------------
    return 0;
}


void exit_message(void)
{
    // do some code here to protect the outputs etc...
    lcd_clrscr();
    printlcd(1, 0, "Catastrophic Failure");
    printlcd(3, 0, "Exiting!!");

    delay_ms(1000);
    goto * 0x00; //bad! hard reset...
}

// char st[60] = "Brad!!!";

// OLD TEST CODE
//wdt_disable();
//PORTE = 0x80;
//printlcd(4,0, "xxxxxICR3 = %u", ICR3);
//delay_ms(1);
//PORTE = 0x00;

//TIFR |= _BV(ICF3);
//exit(1);

// Eeprom stuff
//puts raw data straght into eeprom
//uint8_t EEMEM brads_val = 21;
//uint8_t EEMEM brads_val1  = 12;
//uint8_t EEMEM brads_val2  = 1;
//uint8_t EEMEM brads_val3  = 22;
//eeprom_write_byte(&brads_val, 67);
//  temp = eeprom_read_byte(&brads_val);
// printlcd(1, 1," %d ", temp);
// temp = eeprom_read_byte(&brads_val1);
// printlcd(2, 1," %d ", temp);
// temp = eeprom_read_byte(&brads_val2);

// printlcd(3, 1," %d ", temp);

//char st[60] = "ds";
//eeprom_read_block(&st, &someText, sizeof(someText));
// printlcd(4, 1," %s ", st);
//    delay_ms(10000);

static char shouldUpdateTime()
{
    static char cycles = 0;
     while (cycles++ <= 10)
    {
        uint8_t key = keys_get();

        if (key != 255)
        {
            if (key == 1)
            return 1;
        }
        delay_ms(200); 
    }
    return 0;
}