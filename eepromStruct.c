#include <avr/eeprom.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "util.h"
#include "delay.h"
#include "eepromStruct.h"
#include "hd44780.h"
#include "Program.h"
#include "AutoProgram.h"
#include "pwm.h"
#include "menu.h"


void clearValues();
void UploadProgramsToEeprom();

Program prog1 EEMEM =
{
    1,
    "Heat, then simmer",
    {
        {1, 1, 100, 60*2, "Heat Pan", 0},
        {2, 1, 75, 60*1, "Backing off", 0},
        {3, 1, 30, 60*30, "Simmering", 0},
        {4, 1, 10, 60*10, "Low", 0},
        {5, 1, 0, 1, "Finished", 0}
    },
    pwm_set_A, 
    PollProgram,  
    0,
};

Program prog2 EEMEM =
{
    1,
    "Steak, Sausages",
    {
        {1, 1, 100, 60*2, "Heat Pan", 0},
        {2, 1, 75, 60*7, "Ready to cook!", 0},
        {3, 1, 75, 60*7, "Turn Steaks", 0},
        {4, 1, 10, 60*10, "Warm", 0},
        {5, 1, 0, 1, "Finished", 0}
    },
    pwm_set_A, 
    PollProgram,  
    0,
};

Program prog3 EEMEM =
{
    1,
    "Pancakes",
    {
        {1, 1, 100, 60*2, "Heat Pan", 0},
        {2, 1, 40, 60*1, "Back off", 0},
        {3, 1, 40, 60*15, "Cook Now!", 0},
        {4, 1, 0, 1, "Finished", 0},
        {5, 1, 0, 1, "Finished", 0}
    },
    pwm_set_A, 
    PollProgram,  
    0,
};

Program prog4 EEMEM =
{
    1,
    "Curry",
    {
        {1, 1, 100, 60*2, "Heat Pan", 0},
        {2, 1, 70, 60*4, "Brown Onions etc", 0},
        {3, 1, 40, 60*5, "Reduce", 0},
        {4, 1, 25, 60*15, "Simmer", 0},
        {5, 1, 0, 1, "Finished", 0}
    },
    pwm_set_A, 
    PollProgram,  
    0,
};

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

Program * eepromPrograms[4];

Program * selectedProgram;
static char  programNames[4][20];

void GetProgramNames()
{
    for (int i = 0; i < 4; i ++)
    {
        for(uint8_t j = 0; j < 19; j++)
        {
            programNames[i][j] = 0x20;
        }
        programNames[i][19] = '\0';
        eeprom_read_block(programNames[i], eepromPrograms[i]->Name, strlen(programNames[i]));
    }

}

void SetMenuNames(struct menu * menu)
{
    int ii = 0; 
    while(menu[ii].display_text != NULL & programNames[ii] != NULL)
    {
        menu[ii].display_text = programNames[ii];
        ii++;
    }
    

}

void printProgramNames()
{
    for (int i = 0; i < 4; i ++)
    {
        printf("Program %d: %s\r\n", i+1, programNames[i]);
    }
    
}

uint8_t selectedProgramIndex = 255; 
void programSelectionKeyHandler(uint8_t key)
{
    printf(PSTR("Main Key Handler - KeyPress Detected\r\n"));
    printGlobalValues();
    switch (key)
    {
        case 1:
            printf(PSTR("KH changing from Main to programSelectionKey\r\n"));
            printf(PSTR("Press value 1-4 to select program\r\n"));
            printf(PSTR("Press # to submit\r\n"));
           // program = 0;
           // g_key_handler = programSelectionKey;
            programSelectionDisplay(0);
            break;
    
        default:
            break;
    }
}



void GetProgramFromEeprom(uint8_t index)
{
    assert(index >= 0 && index < 4);
    
    eeprom_read_block(selectedProgram, eepromPrograms[index], sizeof(Program));

}

void LoadProgramsFromEeprom(Program * programs[4]) //todo: This may not work!!
{
    
    delay_ms(10);
    printf(PSTR("Loading programs from EEPROM\r\n"));
    eeprom_read_block(programs[0], &prog1, sizeof(Program));
    eeprom_read_block(programs[1], &prog2, sizeof(Program));
    eeprom_read_block(programs[2], &prog3, sizeof(Program));
    eeprom_read_block(programs[3], &prog4, sizeof(Program));
   
    delay_ms(100);
}

void init( void(*pwmSetterFunc)(char state, char pct), 	void(*pollingFunc)())
{
    printf(PSTR("Allocating space for programs\r\n"));
    eepromPrograms[0] = &prog1;
    eepromPrograms[1] = &prog2;
    eepromPrograms[2] = &prog3;
    eepromPrograms[3] = &prog4;

    selectedProgram = calloc(1, sizeof(Program));
    selectedProgram->pollingFunc = pollingFunc;
    selectedProgram->pwmSetterFunc = pwmSetterFunc;
    GetProgramNames();


    // for(uint8_t i = 0; i < 4; i++)
    // {
    //     programs[i] = calloc(1, sizeof(Program));
    //     programs[i]->pollingFunc = pollingFunc;
    //     programs[i]->pwmSetterFunc = pwmSetterFunc;
    // }
    //  LoadProgramsFromEeprom(programs);
    
}

void UploadProgramToEeprom(Program * program)
{
    printf(PSTR("Writing the following program to prog1 in EEPROM\r\n"));
    printProgram(program);
    eeprom_write_block(program, &prog1, sizeof(Program));
    
    printf(PSTR("Finished!\r\n"));
    delay_ms(100);
}



void UploadProgramsToEeprom(Program * programs[4])
{
    printf(PSTR("Writing the following program to prog1 in EEPROM\r\n"));
    printProgram(programs[0]);
    eeprom_write_block(programs[0], &prog1, sizeof(Program));
    
    printf("Finished!\r\n");
    delay_ms(100);
    printf(PSTR("Writing the following program to prog2 in EEPROM\r\n"));
    printProgram(programs[1]);
    eeprom_write_block(programs[1], &prog2, sizeof(Program));
    printf(PSTR("Finished!\r\n"));
    delay_ms(100);
    printf(PSTR("Writing the following program to prog3 in EEPROM\r\n"));
    printProgram(programs[2]);
    eeprom_write_block(programs[2], &prog3, sizeof(Program));
    printf(PSTR("Finished!\r\n"));
    delay_ms(100);
    printf(PSTR("Writing the following program to prog1 in EEPROM\r\n"));
    printProgram(programs[3]);
    eeprom_write_block(programs[3], &prog4, sizeof(Program));
    printf(PSTR("Finished!\r\n"));
    delay_ms(100);
}

