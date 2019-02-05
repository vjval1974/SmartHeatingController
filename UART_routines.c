//**************************************************************
//******** FUNCTIONS FOR SERIAL COMMUNICATION USING UART *******
//**************************************************************
//Controller:	ATmega128 (Crystal: 16Mhz)
//Compiler:		winAVR (AVR-GCC)
//Author:		CC Dharmani, Chennai (India)
//				www.dharmanitech.com
//Date:			April 2009
//********************************************************

#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "UART_routines.h"
#include <avr/interrupt.h>
#include <string.h>

volatile char index; 
volatile char rxbuf[BUFFER_SIZE];
volatile char line[BUFFER_SIZE];
volatile char lineAvailable;

static void clr_rxbuf()
{
    for(int ii = 0; ii < 32; ii++)
    {
        rxbuf[ii] = 0;
    }   
}



ISR(USART1_RX_vect)
{
    uint8_t c;
   // cli();
    c = UDR1;
    
    if (!bit_is_clear(UCSR1A, FE))
        return;
    
    if (index < sizeof(rxbuf) - 2)
    {
        rxbuf[index++] = c;
        rxbuf[index]   = 0;
        //uart_putchar(c, NULL);
    }

    if ((c == '\n' || c == '\r') && index > 1)
    {

        strcpy(line, rxbuf);
        lineAvailable = 1;
        clr_rxbuf();
        index = 0;
    }
    // else if (c == '\b') // backspace
    // {
    //     if (index > 1) // we just added \b to rxbuf above
    //     {
    //         uart_putc(' ');
    //         uart_putc('\b');
    //         index -= 2; // skip \b we just added and cahr before that
    //         rxbuf[index] = 0;
    //     }
    // }
        
    
   // sei();
}

char uart_get_last_line(char * dest)
{ 
    if (lineAvailable != 1)
    return -1;
    
   // cli();
    
    strlcpy(dest, line, sizeof(line));
    //printf("dest is %s ", dest );
    lineAvailable = 0;
    
   // sei();
    return 1;
}
//**************************************************
//Function initialize UART0
//desired baud rate: 19200
//actual: baud rate:19231 (0.2%)
//char size: 8 bit
//parity: Disabled
//**************************************************

#define UART_BAUD_RATE 19200
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_OSC)/((UART_BAUD_RATE)*16l)-1)
void uart0_init(void)
{
    cli();
    UCSR0B = 0x00; //disable while setting baud rate
    UCSR0A = 0x00;
    UCSR0C = 0x06;
    UBRR0L = 0x67; //set baud rate lo
    UBRR0H = 0x00; //set baud rate hi
    UCSR0B = 0x98;
    sei();
}
    
    void uart_putc(unsigned char c) {
   // wait until UDR ready
	while(!(UCSR1A & (1 << UDRE)));
	UDR1 = c;    // send character
}

void uart_puts (char *s) {
	//  loop until *s != NULL
	while (*s) {
		uart_putc(*s);
		s++;
	}
}

FILE uart_str = FDEV_SETUP_STREAM(uart_putc, receiveByte, _FDEV_SETUP_RW);
void uart1_init(void)
{
    cli();
 // set baud rate
    UBRR1H = (uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_OSC)>>8);
    UBRR1L = (uint8_t)UART_BAUD_CALC(UART_BAUD_RATE,F_OSC);

    // Enable receiver and transmitter; enable RX interrupt
    UCSR1B = (1 << RXEN) | (1 << TXEN) | _BV(RXCIE);

    //asynchronous 8N1
    UCSR1C = (3 << UCSZ0);

    stdin = stdout = &uart_str;
 
    // UCSR1B = 0x00; //disable while setting baud rate
    // UCSR1A = 0x00;
    // UCSR1C = 0x06;
    // UBRR1L = 0x67; //set baud rate lo
    // UBRR1H = 0x00; //set baud rate hi
    // UCSR1B = 0x98;
    // stdin = stdout = &uart_str;
    sei();

}

//**************************************************
//Function to receive a single byte
//*************************************************
unsigned char receiveByte( void )
{
	unsigned char data;
    unsigned char status;
	
	while(!(UCSR0A & (1<<RXC0))); 	// Wait for incomming data
	
	//status = UCSR0A;
	data = UDR0;
		   
	return(data);
}
unsigned char receiveByte1( void )
{
	unsigned char data;
    unsigned char status;
	
	while(!(UCSR1A & (1<<RXC1))); 	// Wait for incomming data
	
	//status = UCSR1A;
    data = UDR1;
		   
	return(data);
}
//***************************************************
//Function to transmit a single byte
//***************************************************
void transmitByte( unsigned char data )
{
	while ( !(UCSR0A & (1<<UDRE0)) )
		; 			                /* Wait for empty transmit buffer */
	UDR0 = data; 			        /* Start transmition */
}
void transmitByte1( unsigned char data )
{
	while ( !(UCSR1A & (1<<UDRE1)) )
		; 			                /* Wait for empty transmit buffer */
	UDR1 = data; 			        /* Start transmition */
}
//***************************************************
//Function to transmit a string in Flash
//***************************************************
void transmitString_F(char* string)
{
  while (pgm_read_byte(&(*string)))
   transmitByte(pgm_read_byte(&(*string++)));
}
void transmitString_F1(char* string)
{
  while (pgm_read_byte(&(*string)))
   transmitByte1(pgm_read_byte(&(*string++)));
}
//***************************************************
//Function to transmit a string in RAM
//***************************************************
void transmitString(char* string)
{
    while (*string)
		transmitByte(*string++);
}
void transmitString1(char* string)
{
    while (*string)
		transmitByte1(*string++);
}
