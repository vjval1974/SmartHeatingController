///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2007, Matthew Pratt
//
// Licensed under the GNU General Public License v3 or greater
//
// Date:  7 Jun 2007
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_H
#define ADC_H

#define ADC_CHAN_0   0x0
#define ADC_CHAN_1   0x1
#define ADC_CHAN_2   0x2
#define ADC_CHAN_3   0x3
#define ADC_CHAN_4   0x4
#define ADC_CHAN_5   0x5
#define ADC_CHAN_6   0x6
#define ADC_CHAN_ULTRA   0x7
#define MAX_NAME_LENGTH 10

unsigned short adc_read(unsigned char adcChan);
void adc_init(unsigned char adcChan);
int adc_calc(uint32_t adc_value, uint32_t scale_max, uint32_t scale_min, 
              char units[MAX_NAME_LENGTH], uint32_t unit_range,
             uint32_t *place_holder);
#endif
