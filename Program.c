#include <avr/eeprom.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <avr/pgmspace.h>
#include "util.h"
#include "delay.h"
#include "eepromStruct.h"
#include "hd44780.h"

#include "Program.h"

void (*g_key_handler)(uint8_t key);
void programSelectionKey(uint8_t key);
void stepSelectionKey(uint8_t key);
void valueSelectionKey(uint8_t key);
void valueUpdateKey(uint8_t key);

void clearValues();

Program * programs[4]; 

static uint8_t program = 0;
static uint8_t step = 0;
static uint8_t value = 0;
static char values[16];

static const char * const pressHash = "Press # to confirm\r\n";
static const char * const pressZero = "press 0 to go back\r\n";
static const char * const lineBreakDash = "---------------------------------------------------\r\n";
static const char * const lineBreakEq = "===================================================\r\n";

void printProgram(Program * program)
{
    printf(lineBreakEq);
    printf("%d:Program Name: %s\r\n", program->Id,  program->Name);
    printf(lineBreakEq);
    for (int ii = 0; ii < 5; ii++)
    {   
        printStep(&program->Steps[ii]);
        printf(lineBreakDash);
    }
    printf(lineBreakEq);

}

void printStep(ProgramStep * step)
{
    printf("StepName: %s\r\nPower: %d%%\r\nTime: %ds\r\n", step->Name, step->PwmPct, step->TimeInSeconds);
}


void printGlobalValues()
{
     printf(lineBreakDash);
    printf( "Program = %d \r\nStep = %d\r\n Value = %d\r\n", program, step, value); 
    printf(lineBreakDash);
}

void programSelectionDisplay(uint8_t key)
{
    
    if (key >= 1 && key <= 4)
    {
        printf(lineBreakDash);
        printf("Program Selection\r\n");
        printf( "[%d]%d: %s\r\n", key, programs[key]->Id, programs[key]->Name);
        printf(pressHash);
        printf(pressZero);
        printf(lineBreakDash);
        
    }
}

void stepSelectionDisplay(uint8_t key)
{
    printf( "P%d S%d V%d\r\n", program, step, value); 
    if (key >= 0 && key <= 4 && step <= 4)
    {
        printf(lineBreakDash);
        printf("Step Selection\r\n");
        printf( "%d: %s\r\n", programs[program]->Steps[key].Id, programs[program]->Steps[key].Name);
        printf( "Power=%02d%% Time=%04ds\r\n", programs[program]->Steps[key].PwmPct, programs[program]->Steps[key].TimeInSeconds);
        printf(pressHash);
        printf(pressZero);
        printf(lineBreakDash);
    }
}

void valueSelectionDisplay(uint8_t key)
{   printf( "P%d S%d V%d\r\n", program, step, value); 
    if (key >= 0 && key <= 1 && program <= 4)
    {
        
    }
}

void mainKey(uint8_t key)
{
    printf(PSTR("Main Key Handler - KeyPress Detected\r\n"));
    printGlobalValues();
    switch (key)
    {
        case 1:
            printf(PSTR("KH changing from Main to programSelectionKey\r\n"));
            printf(PSTR("Press value 1-4 to select program\r\n"));
            printf(PSTR("Press # to submit\r\n"));
            program = 0;
            g_key_handler = programSelectionKey;
            programSelectionDisplay(0);
            break;
    
        default:
            break;
    }
}

void programSelectionKey(uint8_t key)
{
    printf("Program Selection Key Handler - KeyPress Detected\r\n");
    printGlobalValues();
    switch (key)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        {
            
            program = key-1;
            printf("You selected program: %u\r\n", key);
            programSelectionDisplay(key-1);
            break;
        }
        case 0x0b:
        {
            printf("KH changing from programSelectionKey to stepSelectionKey\r\n");
            printf("Press value 1-5 to select step\r\n");
            printf("Press # to submit\r\n");
            step = 0;
            key = 0;
            g_key_handler = stepSelectionKey;
            stepSelectionDisplay(0);
            break;
        }
        case 0:
        {
            printf("KH - 0 Selected, returning to main menu\r\n");
            program = 0;
            step = 0;
            value = 0;
            g_key_handler = mainKey;
        }

        default:
        {
           break;
        }
           
    }
}

void stepSelectionKey(uint8_t key)
{
    printf("Step Selection Key Handler - KeyPress Detected\r\n");
    printGlobalValues();
    switch (key)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        {
            step = key-1;
            stepSelectionDisplay(key-1);
            break;
        }
        case 0x0b:
        {
            printf("KH changing from stepSelectionKey to valueSelectionKey\r\n");
            printf("Press 1 for power\r\n");
            printf("Press 2 for Time\r\n");
            printf("Press # to submit\r\n");
            value = 0;
            
            g_key_handler = valueSelectionKey;
            valueSelectionDisplay(0);
            break;
        }
        case 0:
        {
            printf(PSTR("KH - 0 Selected, returning to main menu\r\n"));
            program = 0;
            step = 0;
            value = 0;
            g_key_handler = mainKey;
        }

        default:
            
            break;
    }
}

void valueSelectionKey(uint8_t key)
{
    printf("Value Selection Key Handler - KeyPress Detected\r\n");
    printGlobalValues();
    switch (key)
    {
        case 1:
        {
            printf("Selected to Edit Power for program %u, Step %u\r\n", program+1, step+1);
            printf("Press # to confirm\r\n");
            value = 1; 
            break;
        }
        case 2:
        {
            printf("Selected to Edit Time for program %u, Step %u\r\n", program+1, step+1);
            printf("Press # to confirm\r\n");
            value = 2;
            break;
        }
        case 0:
        {
            printf("KH - 0 Selected, returning to main menu\r\n");
            program = 0;
            step = 0;
            value = 0;
            g_key_handler = mainKey;
        }
        case 0x0b:
        {
            printf("Entering Value Update  \r\n");
            printf("Press the value on the keypad,\r\n");
            printf("followed by # to confirm \r\n");
            clearValues();
            g_key_handler = valueUpdateKey;
            
            break;
        }

        default:
           
            break;
    }
}
void valueUpdateDisplay(uint8_t key)
{ 
    switch (key)
    {
        case 1:
            printf("you are updating power\r\n");
            /* code */
            break;
        case 2: 
            printf("you are updating step time\r\n");   
        break;
        default:
        printf("how did you get here?!\r\n");
            break;
    }
    printf("Please select each decimal in your value\r\n");
}



void valueUpdateKey(uint8_t key)
{
    
    static uint8_t index = 0;
    printf("Value Update Key Handler - KeyPress Detected\r\n");
    printGlobalValues();
     switch (key)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        {
            itoa(key, &values[index], 10);
            printf("You have selected %d\r\n", key);
            index++;
            
            break;
        }
        case 0x0b:
        {
            if (value == 1)
            {
                programs[program]->Steps[step].PwmPct = (uint8_t) atoi(values);
                printf("updating power to %d: %s\r\n", programs[program]->Steps[step].PwmPct, programs[program]->Steps[step].Name);
            }
            if (value == 2)
            {
                programs[program]->Steps[step].TimeInSeconds = (long) atoi(values);
                printf("Updating time %d: %s\r\n", programs[program]->Steps[step].TimeInSeconds, programs[program]->Steps[step].Name);
            }
            printf("value = %li\r\n", strtol(values, NULL, 10));

            printProgram(programs[program]);

            UploadProgramToEeprom(programs[program]);
            
            //sync(programs[program], &prog1);
            printProgram(programs[program]);
            break;
            }
        case 0x0a:
        {
            program = 0;
            step = 0;
            value = 0;
            g_key_handler = mainKey;
            break;
        }

        default:
           
            break;
    }
    for (int jj = 0; jj < 16; jj++)
    {   
        if (values[jj] != 0)
            printf("%c", values[jj]);
    }
    printf("\r\n");
}

void clearValues()
{
    for (int jj = 0; jj < 16; jj++)
    {  
         values[jj] = 0;
    }
}

