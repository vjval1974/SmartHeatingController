//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 19 Mar 2009
// Email Address: W0085400@mail.connect.usq.edu.au
// 
// Purpose:
//
// RCS $Date: 2012/11/16 03:56:41 $
// RCS $Revision: 1.5 $
// RCS $Source: /media/DataDiskSDA1/Home/brad/Documents/AVR/atmega128_manual_brew/RCS/menu.c,v $
//
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// Included Libraries
//-------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include "menues.h"
//#include "main.h"
#include "menu.h"

#include "keypad.h"
#include "hd44780.h"
#include "hd44780_settings.h"

#include "delay.h"

#include <string.h>

//#include "buttons.h"
//#include "Eeprom.h"
//#include "timer.h"
//#include "idle.h"
//-------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------
#define MAX_DEPTH 15
#define HEIGHT 1

//-------------------------------------------------------------------------
// Global Variables
//-------------------------------------------------------------------------
static struct menu *g_menu[MAX_DEPTH];
static unsigned char g_item = 0;  //top - bottom
static unsigned char g_index = 0; //depth
static unsigned char g_crumbs[MAX_DEPTH];
static void (*g_menu_applet)(unsigned char key) = NULL;

char menu_help_text[] = "";

// ------------------------------------------------------------------------
//*******FUNCTIONS***********
//-------------------------------------------------------------------------
static void menu_update_indicator(void)
{
    char* cc = ">";
    unsigned char line_num = 1;
    
    // printf("g_item = %d\n", g_item);
    //static short old = 0; //we keep track of the old indicator
    //if the next menu is a function that runs without another menu, 
    //we let the user know by changing the indicator
    if (g_menu[g_index][g_item].runfunc && !g_menu[g_index][g_item].next)
        cc = "*"; 
    // printlcd((old+1), 0, " "); // delete location of the old indicator. 
    printlcd(line_num, 0, cc); // place new indicator
    
    // old = g_item; // update old 
        
}

//-------------------------------------------------------------------------
static void menu_run_callback(void)
{
    if (g_index > 0)
    {
        void (*callback)(void) = g_menu[g_index-1][g_crumbs[g_index-1]].runfunc;
        
        if (callback)
            callback();
    }
}

//-------------------------------------------------------------------------
void menu_update(void)
{
    unsigned char ii;
    char text[15];
    lcd_clrscr();
    
    unsigned char xx=1;
    for (ii = g_item ; (ii < (g_item+LCD_DISPLAY_LINES))&&g_menu[g_index][ii].display_text; ii++)
    {
//        printf("ii=%d xx=%d\n", ii, xx);
        // printf_P(PSTR("%d\n"), g_eeprom_data._3_gearbox_overtemp);
        sprintf(text, "%s", g_menu[g_index][ii].display_text);
        if(xx <= LCD_DISPLAY_LINES) 
            printlcd(xx++, 1, text);
    }
    
    menu_update_indicator();
}

//-------------------------------------------------------------------------
void menu_set_root(struct menu *root_menu)
{
    g_menu[0] = root_menu;
    g_index = 0;
    menu_update();
}

//-------------------------------------------------------------------------
static void menu_applet_key(unsigned char key)
{
    g_menu_applet(key);   
   
    if (key == BUTTON_LEFT) 
    {
        menu_clear();
        menu_update();
        g_menu_applet = NULL;
        menu_run_callback();
        set_poll_func(NULL); //if we are polling some function in an 
                             //applet. We must stop when returning from it
    }
   
}
void menu_help(void)
{
    printlcd(1, 0, "  -MENU HELP-   ");
    printlcd_scroll_poll(2, menu_help_text);
}
//-------------------------------------------------------------------------
void menu_key(unsigned char key)
{
   

    if (g_menu_applet)
    {
        menu_applet_key(key);
        return;
    }
    
    //up arrow, move indicator up
    if (key == BUTTON_UP) 
    {
        if (g_item > 0) 
            g_item--;
        menu_update_indicator(); 
        menu_update();
    }
    
    //down arrow, move indicator down
    else if (key == BUTTON_DOWN)
    {
        if (g_menu[g_index][g_item+1].display_text != NULL)
            g_item++;
        else g_item = 0;
        menu_update_indicator();
        menu_update();
    }
    
    //left arrow. go back one level in depth. 
    else if (key == BUTTON_LEFT)
    {
        if (g_index == 0){
            menu_update();
            menu_update_indicator();
            return;
        }

        if (g_index > 0)
            g_index--;
        g_item = 0;
        if (g_index == 0)
            printf_P(PSTR("stepping back to: \"main\"\n")); 
        else
            printf_P(PSTR("stepping back to: \"%s\"\n"), 
                   g_menu[g_index-1][g_item].display_text);  
        set_poll_func(NULL);
        menu_update_indicator();
        menu_update();
        menu_run_callback();
    }
    
    //right arrow. step forward one level in depth
    else if (key == BUTTON_RIGHT)
    {
              
        printf("stepping into: \"%s\"\n", 
               g_menu[g_index][g_item].display_text);          
        
        g_crumbs[g_index] = g_item;
        if (g_menu[g_index][g_item].next && g_index < MAX_DEPTH)
        {
            g_index++;
            g_menu[g_index] = g_menu[g_index-1][g_item].next;
            g_item = 0;
            menu_update();
        }
        else if (g_menu[g_index][g_item].key_handler)
        {
            g_menu_applet = g_menu[g_index][g_item].key_handler;
            menu_clear();
        }
        
        menu_run_callback();
        
    }
   
}

//-------------------------------------------------------------------------
void menu_clear(void)
{
    lcd_clrscr();
}

//-------------------------------------------------------------------------
void menu_run_applet(void (*applet_key_handler)(unsigned char key))
{
    g_menu_applet = applet_key_handler;
}


//-------------------------------------------------------------------------

