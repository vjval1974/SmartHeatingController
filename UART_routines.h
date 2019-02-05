// **************************************************************
// ***  UART_ROUTINES HEADER FILE ******** 		
// **************************************************************

#define TX_NEWLINE		{transmitByte(0x0d); transmitByte(0x0a);}
#define BUFFER_SIZE 32

void uart0_init(void);
void uart1_init(void);
unsigned char receiveByte(void);
unsigned char receiveByte1(void);
void transmitByte(unsigned char);
void transmitByte1(unsigned char);
void transmitString_F(char*);
void transmitString_F1(char*);
void transmitString(char*);
void transmitString1(char*);

void Line(void);
char uart_get_last_line(char * dest);
