/*
Copyright (c) Peter Dannegger <danni@specs.de>
Copyright (c) 2004 Martin Thomas <eversmith@heizung-thomas.de>
Copyright (c) 2007 Matthew Pratt <mattpratt@yahoo.com>

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the <ORGANIZATION> nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "delay.h"
#include "err.h"
#include "timer.h"

#define SENSOR_0  "\x28\xe1\xe6\xb5\x4\x0\x0\xc8"
#define SENSOR_1  "\x10\x6c\xed\x38\x1\x8\x0\xfa"
#define SENSOR_2  " " 
#define SENSOR_3  " " 
#define SENSOR_4  " " 
#define SENSOR_5  " " 
#define SENSOR_6  " " 
#define SENSOR_7  " " 
#define SENSOR_8  " " 
#define SENSOR_9  " " 

#define OW_PIN  PC0
#define OW_IN   PINC
#define OW_OUT  PORTC
#define OW_DDR  DDRC
#define OW_CONF_DELAYOFFSET 2

/* DS18X20 specific values (see datasheet) */
#define DS18S20_ID 0x10
#define DS18X20_CONVERT_T	0x44
#define DS18X20_READ		0xBE
#define DS18X20_SP_SIZE  9

#define OW_MATCH_ROM	0x55
#define	OW_SEARCH_ROM	0xF0
#define OW_SKIP_ROM	    0xCC
#define	OW_SEARCH_FIRST	0xFF		// start new search
#define	OW_PRESENCE_ERR	0xFF
#define	OW_DATA_ERR	    0xFE
#define OW_LAST_DEVICE	0x00		// last device found
//			0x01 ... 0x40: continue searching
// rom-code size including CRC
#define OW_ROMCODE_SIZE 8

#define OW_GET_IN()   ( OW_IN & (1<<OW_PIN))
#define OW_OUT_LOW()  ( OW_OUT &= (~(1 << OW_PIN)) )
#define OW_OUT_HIGH() ( OW_OUT |= (1 << OW_PIN) )
#define OW_DIR_IN()   ( OW_DDR &= (~(1 << OW_PIN )) )
#define OW_DIR_OUT()  ( OW_DDR |= (1 << OW_PIN) )


uint8_t ow_input_pin_state(void)
{
    return OW_GET_IN();
}

void ow_parasite_enable(void)
{
    OW_OUT_HIGH();
    OW_DIR_OUT();
}

void ow_parasite_disable(void)
{
    OW_OUT_LOW();
    OW_DIR_IN();
}

err_t ow_reset(void)
{
    uint8_t sreg;

    OW_OUT_LOW(); // disable internal pull-up (maybe on from parasite)
    OW_DIR_OUT(); // pull OW-Pin low for 480us

    delay_us(480);

    sreg=SREG;
    cli();

    // set Pin as input - wait for clients to pull low
    OW_DIR_IN(); // input

    delay_us(66);

    SREG=sreg; 
    sei();

    // nobody pulled to low, still high
    if (OW_GET_IN() != 0 )		// no one there
        return ERR_OWB_NO_PRESSENCE;

    // after a delay the clients should release the line
    // and input-pin gets back to high due to pull-up-resistor
    delay_us(480-66);
    if( OW_GET_IN() == 0 )		// short circuit
        return ERR_OWB_SHORT_CIRCUIT;

    
    return ERR_NONE;
}


/* Timing issue when using runtime-bus-selection (!OW_ONE_BUS):
   The master should sample at the end of the 15-slot after initiating
   the read-time-slot. The variable bus-settings need more
   cycles than the constant ones so the delays had to be shortened
   to achive a 15uS overall delay
   Setting/clearing a bit in I/O Register needs 1 cyle in OW_ONE_BUS
   but around 14 cyles in configureable bus (us-Delay is 4 cyles per uS) */
uint8_t ow_bit_io( uint8_t b )
{
	uint8_t sreg;

	sreg=SREG;
	cli();

        OW_OUT_LOW();
	OW_DIR_OUT(); // drive bus low

	delay_us(2); // Recovery-Time wuffwuff was 1
	if ( b ) OW_DIR_IN(); // if bit is 1 set bus high (by ext. pull-up)

	// wuffwuff delay was 15uS-1 see comment above
	delay_us(15-1-OW_CONF_DELAYOFFSET);

	if( OW_GET_IN() == 0 ) b = 0;  // sample at end of read-timeslot

	delay_us(60-15);
	OW_DIR_IN();

	SREG=sreg; 
        sei();

        delay_us(30);
	return b;
}


uint8_t ow_byte_wr( uint8_t b )
{
	uint8_t i = 8, j;

	do {
		j = ow_bit_io( b & 1 );
		b >>= 1;
		if( j ) b |= 0x80;
	} while( --i );

	return b;
}


uint8_t ow_byte_rd( void )
{
  // read by sending 0xff (a dontcare?)
  return ow_byte_wr( 0xFF );
}

err_t ow_command( uint8_t command, uint8_t *id )
{
    uint8_t i;
    err_t err;

    err = ow_reset();
    
    if (err != ERR_NONE)
        return err;

    if( id ) {
        ow_byte_wr( OW_MATCH_ROM );			// to a single device
        i = OW_ROMCODE_SIZE;
        do {
            ow_byte_wr( *id );
            id++;
        } while( --i );
    }
    else {
        ow_byte_wr( OW_SKIP_ROM );			// to all devices
    }

    ow_byte_wr( command );
    return ERR_NONE;
}


static err_t         g_errs[10];
static unsigned char g_temps[10] = {0,0,0,0,0,0,0,0,0,0};

err_t ds1820_start_convert(void)
{
    err_t err = ow_reset();
    ow_byte_wr( OW_SKIP_ROM );			// to all devices
    ow_byte_wr( 0x44 );
    ow_parasite_enable();
    
    return err;
}

#if 0
unsigned char readTemps(void)
{
    uart_putc(search_sensors());

    err_t err =  ow_reset();
    if (err != ERR_NONE)
        return 0;
    ow_byte_wr( OW_SKIP_ROM );			// to all devices
    ow_byte_wr( 0x44 );
    ow_parasite_enable();

    delay_ms(750);

    ow_reset();
    ow_byte_wr( OW_SKIP_ROM );			// to all devices
    ow_byte_wr(0xbe);

    return ow_byte_rd();
}
#endif

err_t ds1820_get_temp(uint8_t index, unsigned char *temp)
{
    if (index < sizeof(g_temps))
    {
        *temp = g_temps[index];
        return  g_errs[index];
    }
    return ERR_INDEX_OUT_OF_BOUNDS;
}

static uint8_t ds1820_read_temp(const char *id, unsigned char *temp)
{
    uint8_t i;
    uint8_t sp[DS18X20_SP_SIZE] = {0};
    err_t err;

    *temp = 0;
   
    err = ow_command(DS18X20_READ, (uint8_t*)id);
    if (err == ERR_NONE)
    {
        for ( i=0 ; i< DS18X20_SP_SIZE; i++ ) {
                sp[i]=ow_byte_rd();
        }
        //if ( crc8( &sp[0], DS18X20_SP_SIZE ) ) 
        //          return DS18X20_ERROR_CRC;	if (sp[0] == 0xff)
        if (sp[0] == 0xff)
	{
            err = ERR_OWB_NO_PRESSENCE;
            
	}
        else
        {
            *temp = sp[0] >> 1;
        }
    }
    return err;
}

void ds1820_process(void)
{
    static uint32_t last = 0;
    uint32_t now = timer_get();

    if (now - last >= _THIRD_SECONDS_IN_TICKS(2))
    {
        last = now;
        g_errs[0] = ds1820_read_temp(SENSOR_0,  &g_temps[0]);
        g_errs[1] = ds1820_read_temp(SENSOR_1,  &g_temps[1]);
        g_errs[2] = ds1820_read_temp(SENSOR_2,  &g_temps[2]);
        g_errs[3] = ds1820_read_temp(SENSOR_3,  &g_temps[3]);
        g_errs[4] = ds1820_read_temp(SENSOR_4,  &g_temps[4]);
        g_errs[5] = ds1820_read_temp(SENSOR_5,  &g_temps[5]);
        g_errs[6] = ds1820_read_temp(SENSOR_6,  &g_temps[6]);
        g_errs[7] = ds1820_read_temp(SENSOR_7,  &g_temps[7]);
        g_errs[8] = ds1820_read_temp(SENSOR_8,  &g_temps[8]);
        g_errs[9] = ds1820_read_temp(SENSOR_9,  &g_temps[9]);
        
        
            
        ds1820_start_convert();

    }
}


#if 1 // ds1820 search code

uint8_t ow_rom_search( uint8_t diff, uint8_t *id )
{
    uint8_t i, j, next_diff;
    uint8_t b;

    if( ow_reset() ) return OW_PRESENCE_ERR;	// error, no device found

    ow_byte_wr( OW_SEARCH_ROM );			// ROM search command
    next_diff = OW_LAST_DEVICE;			// unchanged on last device

    i = OW_ROMCODE_SIZE * 8;					// 8 bytes

    do {
        j = 8;					// 8 bits
        do {
            b = ow_bit_io( 1 );			// read bit
            if( ow_bit_io( 1 ) ) {			// read complement bit
                if( b )					// 11
                {
                    return OW_DATA_ERR;			// data error
                    
                }
            }
            else {
                if( !b ) {				// 00 = 2 devices
                    if( diff > i || ((*id & 1) && diff != i) ) {
                        b = 1;				// now 1
                        next_diff = i;			// next pass 0
                    }
                }
            }
            ow_bit_io( b );     			// write bit
            *id >>= 1;
            if( b ) *id |= 0x80;			// store bit

            i--;

        } while( --j );

        id++;					// next byte

    } while( i );

    return next_diff;				// to continue search
}


/* find DS18X20 Sensors on 1-Wire-Bus
   input/ouput: diff is the result of the last rom-search
   output: id is the rom-code of the sensor found */
void DS18X20_find_sensor(uint8_t *diff, uint8_t id[])
{
    for (;;) {
        *diff = ow_rom_search( *diff, &id[0] );
        if (*diff == OW_PRESENCE_ERR || *diff == OW_DATA_ERR ||
            *diff == OW_LAST_DEVICE) return;
        if (id[0] == DS18S20_ID) return;
    }
}


#define MAXSENSORS 10
uint8_t gSensorIDs[MAXSENSORS][OW_ROMCODE_SIZE];

uint8_t ds1820_search(void)
{
    uint8_t i;
    uint8_t id[OW_ROMCODE_SIZE];
    uint8_t diff, nSensors;

    nSensors = 0;

    for( diff = OW_SEARCH_FIRST;
         diff != OW_LAST_DEVICE && nSensors < MAXSENSORS ; )
    {
        DS18X20_find_sensor( &diff, &id[0] );

        if( diff == OW_PRESENCE_ERR ) {
            printf("No Sensor Found \n\r");
            break;
        }

        if( diff == OW_DATA_ERR ) {
            printf("Bus Error! /r/n");
            break;
        }

        printf("Sensor %d = ", nSensors);
        for (i=0;i<OW_ROMCODE_SIZE;i++)
        {
            gSensorIDs[nSensors][i]=id[i];
            printf("\\x%x", id[i]);
                      
        }
        printf("\n");
        nSensors++;
        
    }
    
    return nSensors;
}

#endif
