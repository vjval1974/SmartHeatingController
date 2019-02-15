///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010, Brad Goold, All Rights Reserved.
//
// Authors: Brad Goold
//
// Date: 18 Dec 2010
//
// RCS $Date: 2012/04/02 22:42:25 $
// RCS $Revision: 1.1 $
// RCS $Source: /home/brad/Documents/AVR/atmega128_flow/RCS/timer2.h,v $
// RCS $Log: timer2.h,v $
// RCS Revision 1.1  2012/04/02 22:42:25  brad
// RCS Initial revision
// RCS
// RCS Revision 1.1  2010/12/29 00:42:17  brad
// RCS Initial revision
// RCS///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER2_H
#define TIMER2_H

volatile uint8_t final_compare_val;
volatile char final_compare_enable;
volatile uint32_t t2_wraps;
void timer2_init(void);
uint32_t timer2_get(void);
void timer2_reset(void);

#endif