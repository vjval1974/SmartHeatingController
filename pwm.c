//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 15 Nov 2011
// Email Address: W0085400@umail.usq.edu.au
// 
// Purpose: To set up pwm on port E pins 3, 4, 5 at a desired freq and pw
// Pre: Timer 3 must be free to use and the pins available
//      Desired frequency and output pins required on initialisation
//      Desired width percentage required on setting up. 
// Post: PWM at set frequency
// RCS $Date: 2019/02/01 11:15:08 $
// RCS $Revision: 1.1 $
// RCS $Source: /media/brad/Brad_G/BackupArchive/May2016/RestoredHomeDir/media/DataDiskSDA1/Home/brad/Documents/Microcontrollers/AVR/ProtoBox/RCS/pwm.c,v $
// RCS $Log: pwm.c,v $
// RCS Revision 1.1  2019/02/01 11:15:08  brad
// RCS Initial revision
// RCS
// RCS Revision 1.1  2011/11/15 06:19:43  brad
// RCS Initial revision
// RCS
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// Included Libraries
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pwm.h"

volatile long unsigned top = 0;

void init_pwm(long unsigned frequency, char A, char B, char C)
{
//Set the Data Direction on the required pins to output. 

    if (A){
        DDRE |= _BV(3);    //set for using OC3A
        PORTE &= ~_BV(3);
    }
    if (B){
        DDRE |= _BV(4);    //set for using OC3B
         PORTE &= ~_BV(4);
    }
    if (C){
        DDRE |= _BV(5);    //set for using OC3C
        PORTE &= ~_BV(5);
    }
    
    long unsigned x = 16000000/(2*frequency);
    long unsigned int pvals[5] = {1, 8, 64, 256, 1024};
    long unsigned int prescaler = 1;
    int i;
    for (i = 0; i < 5; i++)
    {
        
        if ((x / pvals[i]) <= 0xFFFF){
            ICR3 = top = x / pvals[i];
            prescaler = pvals[i];
            TCCR3B &= ~0x07; //clear bits
            TCCR3B |= i+1; //set the prescale bits to the value
                           //required.
            //  printf("i = %d, x/pv= %lu\n", i, x/pvals[i]);
            i = 5;
        }
        //
    }
    
    

    //Set the Control Registers to perform the operation required)
    //(Comment out the lines that are not required)
    TCCR3A |= _BV(COM3A1);     //Control the output compare pins PORT E3
//    TCCR3A |= _BV(COM3A0);     //Control the output compare pins PORT E3
    TCCR3A |= _BV(COM3B1);     //Control the output compare pins PORT E4
//    TCCR3A |= _BV(COM3B0);     //Control the output compare pins PORT E4
    TCCR3A |= _BV(COM3C1);     //Control the output compare pins PORT E5
//    TCCR3A |= _BV(COM3C0);     //Control the output compare pins PORT E5
    
 
    //Phas correct PWM with top = ICR3
//    TCCR3A |= _BV(WGM30);      //Waveform Generation Mode Selection    
    TCCR3A |= _BV(WGM31);      //Waveform Generation Mode Selection
//   TCCR3B |= _BV(WGM32);      //Waveform Generation Mode Selection
    TCCR3B |= _BV(WGM33);      //Waveform Generation Mode Selection

//
    printf("PWM Setup:");
    printf("prescaler set to %lu\n",prescaler);
    printf("frequency set to %lu\nhz", frequency); 
    printf("top value (ICR3) set to %lu\n", top);
}
long unsigned pwm_get_A_pct()
{
	unsigned long x = OCR3A;
	if (x == 0) return 0;
	unsigned long y = 100 * x;
	if (y / top == 100) return 100;
	
	return y / top + 1;
}
void pwm_set_A(char state, char pct){
    if (!state){
        OCR3A = 0;
        return;
    }
    long unsigned x = (pct*top)/100;
    OCR3A = x;
}

void pwm_set_B(char state, char pct){
    if (!state){
        OCR3B = 0;
        return;
    }
    long unsigned x = (pct*top)/100;
    OCR3B = x;
}

void pwm_set_C(char state, char pct){
    if (!state){
        OCR3C = 0;
        return;
    }
    long unsigned x = (pct*top)/100;
    OCR3C = x;
}

void pctToBar(uint8_t pct, char row[16])
{
    int numCells = (pct * TOTALCELLS) / 100;

    int ii;
    for (ii = 0; ii < TOTALCELLS; ii++)
    {
        if (ii < numCells)
            row[ii] = 0xff;
        else
            row[ii] = ' ';
    }
}
