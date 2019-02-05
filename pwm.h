///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011, Brad Goold, All Rights Reserved.
//
// Authors: Brad Goold
//
// Date: 15 Nov 2011
//
// RCS $Date: 2019/02/01 11:15:08 $
// RCS $Revision: 1.1 $
// RCS $Source: /media/brad/Brad_G/BackupArchive/May2016/RestoredHomeDir/media/DataDiskSDA1/Home/brad/Documents/Microcontrollers/AVR/ProtoBox/RCS/pwm.h,v $
// RCS $Log: pwm.h,v $
// RCS Revision 1.1  2019/02/01 11:15:08  brad
// RCS Initial revision
// RCS
// RCS Revision 1.1  2011/11/15 06:19:43  brad
// RCS Initial revision
// RCS///////////////////////////////////////////////////////////////////////////////

#ifndef PWM_H
#define PWM_H
#define ON 1
#define OFF 0
#define TOTALCELLS 20
// initialise with the desired frequency and the channels OC3A, B, or
// C. 0 = off. eg: init_pwm(45, ON, ON, OFF) 
void init_pwm(long unsigned frequency, char A, char B, char C);

// set by giving state (OFF or ON) and pwm percentage 
// eg pwm_set_A(ON, 33);
void pwm_set_A(char state, char pct);
void pwm_set_B(char state, char pct);
void pwm_set_C(char state, char pct);
long unsigned pwm_get_A_pct();
void pctToBar(uint8_t pct, char row[16]);

#endif
