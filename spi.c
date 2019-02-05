//-------------------------------------------------------------------------
// Author: Brad Goold
// Date: 16 Nov 2009
// Email Address: W0085400@umail.usq.edu.au
// 
// Purpose:To interface the Atmega128 with SPI peripherals. 
//-------------------------------------------------------------------------
/* The 74HC595 8bit serial to parallel uses the SPI interface which can be 
configured on the AVR. 
Using the ETT Mini board, connect the pins so that they correspond with the
datasheet. The latch pin must be used to tell the chip that the data can be
shifted from the storage register to the output register. This is done by
pulling the pin high after the shift transfer is completed. 
*/

//-------------------------------------------------------------------------
// Included Libraries
//-------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "spi.h"
#include "hd44780.h"


#define DDR_SPI DDRB    //some macro's for readability
#define SPI_PORT PORTB


const int DD_MOSI = 2; //Data Direction register locations of pins. 
const int DD_MISO = 3; 
const int DD_SCK  = 1;
const int DD_LATCH =7;

void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK) |(1<<DD_LATCH);
	/* Enable SPI, Master, set clock rate fck/128 */
	SPCR = (1<<SPE)|(1<<MSTR); //|(1<<SPR0) | (1<<SPR1);
	SPSR = (1<<SPI2X);
}

void SPI_MasterTransmit(char cData)
{
	
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	//printlcd(3, 0, "             ");
	SPI_PORT |= 0x80;
}

void SPI_SlaveInit(void)
{
	/* Set MISO output, all others input */
	DDR_SPI = (1<<DD_MISO);
	/* Enable SPI */
	SPCR = (1<<SPE);
}

char SPI_SlaveReceive(void)
{
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)));
	/* Return data register */
	return SPDR;
}

void SPI_74HC595_TX(char cData){
SPI_PORT &= ~(1<<DD_LATCH); //make sure the latch pin is low before tx
SPI_MasterTransmit(cData);
SPI_PORT |= (1<<DD_LATCH);  //pull the latch pin high to send data to outputs
delay_us(100); 
SPI_PORT &= ~(1<<DD_LATCH); //return latch pin low. 
}
