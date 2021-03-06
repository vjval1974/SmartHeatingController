/*****************************************************************************
Title  :   HD44780 Library
Author :   SA Development
Version:   1.11
*****************************************************************************/

#include "/usr/lib/avr/include/avr/sfr_defs.h"
#include "hd44780.h"
#include "/usr/lib/avr/include/avr/pgmspace.h"
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#if (USE_ADELAY_LIBRARY==1)
  #include "delay.h"
  #include "timer.h"
#else
  #define Delay_ns(__ns) \
    if((unsigned long) (F_CPU/1000000000.0 * __ns) != F_CPU/1000000000.0 * __ns)\
          __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000000000.0 * __ns)+1);\
    else __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000000000.0 * __ns))
  #define Delay_us(__us) \
    if((unsigned long) (F_CPU/1000000.0 * __us) != F_CPU/1000000.0 * __us)\
          __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000000.0 * __us)+1);\
    else __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000000.0 * __us))
  #define Delay_ms(__ms) \
    if((unsigned long) (F_CPU/1000.0 * __ms) != F_CPU/1000.0 * __ms)\
          __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000.0 * __ms)+1);\
    else __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000.0 * __ms))
  #define Delay_s(__s) \
    if((unsigned long) (F_CPU/1.0 * __s) != F_CPU/1.0 * __s)\
          __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1.0 * __s)+1);\
    else __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1.0 * __s))
#endif

#if !defined(LCD_BITS) || (LCD_BITS!=4 && LCD_BITS!=8)
  #error LCD_BITS is not defined or not valid.
#endif

#if !defined(WAIT_MODE) || (WAIT_MODE!=0 && WAIT_MODE!=1)
  #error WAIT_MODE is not defined or not valid.
#endif

#if !defined(RW_LINE_IMPLEMENTED) || (RW_LINE_IMPLEMENTED!=0 && RW_LINE_IMPLEMENTED!=1)
  #error RW_LINE_IMPLEMENTED is not defined or not valid.
#endif

#if (WAIT_MODE==1 && RW_LINE_IMPLEMENTED!=1)
  #error WAIT_MODE=1 requires RW_LINE_IMPLEMENTED=1.
#endif

#if !defined(LCD_DISPLAYS) || (LCD_DISPLAYS<1) || (LCD_DISPLAYS>4)
  #error LCD_DISPLAYS is not defined or not valid.
#endif

// Constants/Macros
#define PIN(x) (*(&x - 2))           // Address of Data Direction Register of Port X
#define DDR(x) (*(&x - 1))           // Address of Input Register of Port X

//PORT defines
#define lcd_rs_port_low() LCD_RS_PORT&=~_BV(LCD_RS_PIN)
#if RW_LINE_IMPLEMENTED==1
  #define lcd_rw_port_low() LCD_RW_PORT&=~_BV(LCD_RW_PIN)
#endif
#define lcd_db0_port_low() LCD_DB0_PORT&=~_BV(LCD_DB0_PIN)
#define lcd_db1_port_low() LCD_DB1_PORT&=~_BV(LCD_DB1_PIN)
#define lcd_db2_port_low() LCD_DB2_PORT&=~_BV(LCD_DB2_PIN)
#define lcd_db3_port_low() LCD_DB3_PORT&=~_BV(LCD_DB3_PIN)
#define lcd_db4_port_low() LCD_DB4_PORT&=~_BV(LCD_DB4_PIN)
#define lcd_db5_port_low() LCD_DB5_PORT&=~_BV(LCD_DB5_PIN)
#define lcd_db6_port_low() LCD_DB6_PORT&=~_BV(LCD_DB6_PIN)
#define lcd_db7_port_low() LCD_DB7_PORT&=~_BV(LCD_DB7_PIN)

#define lcd_rs_port_high() LCD_RS_PORT|=_BV(LCD_RS_PIN)
#if RW_LINE_IMPLEMENTED==1
  #define lcd_rw_port_high() LCD_RW_PORT|=_BV(LCD_RW_PIN)
#endif
#define lcd_db0_port_high() LCD_DB0_PORT|=_BV(LCD_DB0_PIN)
#define lcd_db1_port_high() LCD_DB1_PORT|=_BV(LCD_DB1_PIN)
#define lcd_db2_port_high() LCD_DB2_PORT|=_BV(LCD_DB2_PIN)
#define lcd_db3_port_high() LCD_DB3_PORT|=_BV(LCD_DB3_PIN)
#define lcd_db4_port_high() LCD_DB4_PORT|=_BV(LCD_DB4_PIN)
#define lcd_db5_port_high() LCD_DB5_PORT|=_BV(LCD_DB5_PIN)
#define lcd_db6_port_high() LCD_DB6_PORT|=_BV(LCD_DB6_PIN)
#define lcd_db7_port_high() LCD_DB7_PORT|=_BV(LCD_DB7_PIN)

#define lcd_rs_port_set(value) if (value) lcd_rs_port_high(); else lcd_rs_port_low();
#if RW_LINE_IMPLEMENTED==1
  #define lcd_rw_port_set(value) if (value) lcd_rw_port_high(); else lcd_rw_port_low();
#endif
#define lcd_db0_port_set(value) if (value) lcd_db0_port_high(); else lcd_db0_port_low();
#define lcd_db1_port_set(value) if (value) lcd_db1_port_high(); else lcd_db1_port_low();
#define lcd_db2_port_set(value) if (value) lcd_db2_port_high(); else lcd_db2_port_low();
#define lcd_db3_port_set(value) if (value) lcd_db3_port_high(); else lcd_db3_port_low();
#define lcd_db4_port_set(value) if (value) lcd_db4_port_high(); else lcd_db4_port_low();
#define lcd_db5_port_set(value) if (value) lcd_db5_port_high(); else lcd_db5_port_low();
#define lcd_db6_port_set(value) if (value) lcd_db6_port_high(); else lcd_db6_port_low();
#define lcd_db7_port_set(value) if (value) lcd_db7_port_high(); else lcd_db7_port_low();

//PIN defines
#define lcd_db0_pin_get() (((PIN(LCD_DB0_PORT) & _BV(LCD_DB0_PIN))==0)?0:1)
#define lcd_db1_pin_get() (((PIN(LCD_DB1_PORT) & _BV(LCD_DB1_PIN))==0)?0:1)
#define lcd_db2_pin_get() (((PIN(LCD_DB2_PORT) & _BV(LCD_DB2_PIN))==0)?0:1)
#define lcd_db3_pin_get() (((PIN(LCD_DB3_PORT) & _BV(LCD_DB3_PIN))==0)?0:1)
#define lcd_db4_pin_get() (((PIN(LCD_DB4_PORT) & _BV(LCD_DB4_PIN))==0)?0:1)
#define lcd_db5_pin_get() (((PIN(LCD_DB5_PORT) & _BV(LCD_DB5_PIN))==0)?0:1)
#define lcd_db6_pin_get() (((PIN(LCD_DB6_PORT) & _BV(LCD_DB6_PIN))==0)?0:1)
#define lcd_db7_pin_get() (((PIN(LCD_DB7_PORT) & _BV(LCD_DB7_PIN))==0)?0:1)

//DDR defines
#define lcd_rs_ddr_low() DDR(LCD_RS_PORT)&=~_BV(LCD_RS_PIN)
#if RW_LINE_IMPLEMENTED==1
  #define lcd_rw_ddr_low() DDR(LCD_RW_PORT)&=~_BV(LCD_RW_PIN)
#endif
#define lcd_db0_ddr_low() DDR(LCD_DB0_PORT)&=~_BV(LCD_DB0_PIN)
#define lcd_db1_ddr_low() DDR(LCD_DB1_PORT)&=~_BV(LCD_DB1_PIN)
#define lcd_db2_ddr_low() DDR(LCD_DB2_PORT)&=~_BV(LCD_DB2_PIN)
#define lcd_db3_ddr_low() DDR(LCD_DB3_PORT)&=~_BV(LCD_DB3_PIN)
#define lcd_db4_ddr_low() DDR(LCD_DB4_PORT)&=~_BV(LCD_DB4_PIN)
#define lcd_db5_ddr_low() DDR(LCD_DB5_PORT)&=~_BV(LCD_DB5_PIN)
#define lcd_db6_ddr_low() DDR(LCD_DB6_PORT)&=~_BV(LCD_DB6_PIN)
#define lcd_db7_ddr_low() DDR(LCD_DB7_PORT)&=~_BV(LCD_DB7_PIN)

#define lcd_rs_ddr_high() DDR(LCD_RS_PORT)|=_BV(LCD_RS_PIN)
#if RW_LINE_IMPLEMENTED==1
  #define lcd_rw_ddr_high() DDR(LCD_RW_PORT)|=_BV(LCD_RW_PIN)
#endif
#define lcd_db0_ddr_high() DDR(LCD_DB0_PORT)|=_BV(LCD_DB0_PIN)
#define lcd_db1_ddr_high() DDR(LCD_DB1_PORT)|=_BV(LCD_DB1_PIN)
#define lcd_db2_ddr_high() DDR(LCD_DB2_PORT)|=_BV(LCD_DB2_PIN)
#define lcd_db3_ddr_high() DDR(LCD_DB3_PORT)|=_BV(LCD_DB3_PIN)
#define lcd_db4_ddr_high() DDR(LCD_DB4_PORT)|=_BV(LCD_DB4_PIN)
#define lcd_db5_ddr_high() DDR(LCD_DB5_PORT)|=_BV(LCD_DB5_PIN)
#define lcd_db6_ddr_high() DDR(LCD_DB6_PORT)|=_BV(LCD_DB6_PIN)
#define lcd_db7_ddr_high() DDR(LCD_DB7_PORT)|=_BV(LCD_DB7_PIN)

#define lcd_rs_ddr_set(value) if (value) lcd_rs_ddr_high(); else lcd_rs_ddr_low();
#if RW_LINE_IMPLEMENTED==1
  #define lcd_rw_ddr_set(value) if (value) lcd_rw_ddr_high(); else lcd_rw_ddr_low();
#endif
#define lcd_db0_ddr_set(value) if (value) lcd_db0_ddr_high(); else lcd_db0_ddr_low();
#define lcd_db1_ddr_set(value) if (value) lcd_db1_ddr_high(); else lcd_db1_ddr_low();
#define lcd_db2_ddr_set(value) if (value) lcd_db2_ddr_high(); else lcd_db2_ddr_low();
#define lcd_db3_ddr_set(value) if (value) lcd_db3_ddr_high(); else lcd_db3_ddr_low();
#define lcd_db4_ddr_set(value) if (value) lcd_db4_ddr_high(); else lcd_db4_ddr_low();
#define lcd_db5_ddr_set(value) if (value) lcd_db5_ddr_high(); else lcd_db5_ddr_low();
#define lcd_db6_ddr_set(value) if (value) lcd_db6_ddr_high(); else lcd_db6_ddr_low();
#define lcd_db7_ddr_set(value) if (value) lcd_db7_ddr_high(); else lcd_db7_ddr_low();

#if (WAIT_MODE==1 && RW_LINE_IMPLEMENTED==1)
static unsigned char PrevCmdInvolvedAddressCounter=0;
#endif

#if (LCD_DISPLAYS>1)
static unsigned char ActiveDisplay=1;
#endif

static inline void lcd_e_port_low()
{
  #if (LCD_DISPLAYS>1)
  switch (ActiveDisplay)
    {
      case 2 : LCD_E2_PORT&=~_BV(LCD_E2_PIN);
               break;
      #if (LCD_DISPLAYS>=3)
      case 3 : LCD_E3_PORT&=~_BV(LCD_E3_PIN);
               break;
      #endif
      #if (LCD_DISPLAYS==4)
      case 4 : LCD_E4_PORT&=~_BV(LCD_E4_PIN);
               break;
      #endif
      default :
  #endif
                LCD_E_PORT&=~_BV(LCD_E_PIN);
  #if (LCD_DISPLAYS>1)
    }
  #endif
}

static inline void lcd_e_port_high()
{
  #if (LCD_DISPLAYS>1)
  switch (ActiveDisplay)
    {
      case 2 : LCD_E2_PORT|=_BV(LCD_E2_PIN);
               break;
      #if (LCD_DISPLAYS>=3)
      case 3 : LCD_E3_PORT|=_BV(LCD_E3_PIN);
               break;
      #endif
      #if (LCD_DISPLAYS==4)
      case 4 : LCD_E4_PORT|=_BV(LCD_E4_PIN);
               break;
      #endif
      default :
  #endif
                LCD_E_PORT|=_BV(LCD_E_PIN);
  #if (LCD_DISPLAYS>1)
    }
  #endif
}

static inline void lcd_e_ddr_low()
{
  #if (LCD_DISPLAYS>1)
  switch (ActiveDisplay)
    {
      case 2 : DDR(LCD_E2_PORT)&=~_BV(LCD_E2_PIN);
               break;
      #if (LCD_DISPLAYS>=3)
      case 3 : DDR(LCD_E3_PORT)&=~_BV(LCD_E3_PIN);
               break;
      #endif
      #if (LCD_DISPLAYS==4)
      case 4 : DDR(LCD_E4_PORT)&=~_BV(LCD_E4_PIN);
               break;
      #endif
      default :
  #endif
                DDR(LCD_E_PORT)&=~_BV(LCD_E_PIN);
  #if (LCD_DISPLAYS>1)
    }
  #endif
}

static inline void lcd_e_ddr_high()
{
  #if (LCD_DISPLAYS>1)
  switch (ActiveDisplay)
    {
      case 2 : DDR(LCD_E2_PORT)|=_BV(LCD_E2_PIN);
               break;
      #if (LCD_DISPLAYS>=3)
      case 3 : DDR(LCD_E3_PORT)|=_BV(LCD_E3_PIN);
               break;
      #endif
      #if (LCD_DISPLAYS==4)
      case 4 : DDR(LCD_E4_PORT)|=_BV(LCD_E4_PIN);
               break;
      #endif
      default :
  #endif
                DDR(LCD_E_PORT)|=_BV(LCD_E_PIN);
  #if (LCD_DISPLAYS>1)
    }
  #endif
}

static int g_line = 1;

/*************************************************************************
loops while lcd is busy, returns address counter
*************************************************************************/
#if (WAIT_MODE==1 && RW_LINE_IMPLEMENTED==1)
static uint8_t lcd_read(uint8_t rs);

static void lcd_waitbusy(void)
  {
    register uint8_t c;
    unsigned int ul1=0;

    while ( ((c=lcd_read(0)) & (1<<LCD_BUSY)) && ul1<((F_CPU/16384>=16)?F_CPU/16384:16))     // Wait Until Busy Flag is Cleared
      ul1++;
  }
#endif


/*************************************************************************
Low-level function to read byte from LCD controller
Input:    rs     1: read data
                 0: read busy flag / address counter
Returns:  byte read from LCD controller
*************************************************************************/
#if RW_LINE_IMPLEMENTED==1
static uint8_t lcd_read(uint8_t rs)
  {
    uint8_t data;
    
    #if (WAIT_MODE==1 && RW_LINE_IMPLEMENTED==1)
    if (rs)
      lcd_waitbusy();
      if (PrevCmdInvolvedAddressCounter)
        {
          delay_us(5);
          PrevCmdInvolvedAddressCounter=0;
        }
    #endif

    if (rs)
      {
        lcd_rs_port_high();                             // RS=1: Read Data
        #if (WAIT_MODE==1 && RW_LINE_IMPLEMENTED==1)
        PrevCmdInvolvedAddressCounter=1;
        #endif
      }
    else lcd_rs_port_low();                           // RS=0: Read Busy Flag

    
    lcd_rw_port_high();                               // RW=1: Read Mode

    #if LCD_BITS==4
      lcd_db7_ddr_low();                              // Configure Data Pins as Input
      lcd_db6_ddr_low();
      lcd_db5_ddr_low();
      lcd_db4_ddr_low();

      lcd_e_port_high();                              // Read High Nibble First
      delay_us(1);

      data=lcd_db4_pin_get() << 4 | lcd_db5_pin_get() << 5 |
           lcd_db6_pin_get() << 6 | lcd_db7_pin_get() << 7;

      lcd_e_port_low();
      delay_us(1);

      lcd_e_port_high();                              // Read Low Nibble
      delay_us(1);

      data|=lcd_db4_pin_get() << 0 | lcd_db5_pin_get() << 1 |
            lcd_db6_pin_get() << 2 | lcd_db7_pin_get() << 3;

      lcd_e_port_low();

      lcd_db7_ddr_high();                             // Configure Data Pins as Output
      lcd_db6_ddr_high();
      lcd_db5_ddr_high();
      lcd_db4_ddr_high();

      lcd_db7_port_high();                            // Pins High (Inactive)
      lcd_db6_port_high();
      lcd_db5_port_high();
      lcd_db4_port_high();
    #else //using 8-Bit-Mode
      lcd_db7_ddr_low();                              // Configure Data Pins as Input
      lcd_db6_ddr_low();
      lcd_db5_ddr_low();
      lcd_db4_ddr_low();
      lcd_db3_ddr_low();
      lcd_db2_ddr_low();
      lcd_db1_ddr_low();
      lcd_db0_ddr_low();

      lcd_e_port_high();
      delay_us(1);

      data=lcd_db7_pin_get() << 7 | lcd_db6_pin_get() << 6 |
           lcd_db5_pin_get() << 5 | lcd_db4_pin_get() << 4 |
           lcd_db3_pin_get() << 3 | lcd_db2_pin_get() << 2 |
           lcd_db1_pin_get() << 1 | lcd_db0_pin_get();

      lcd_e_port_low();

      lcd_db7_ddr_high();                             // Configure Data Pins as Output
      lcd_db6_ddr_high();
      lcd_db5_ddr_high();
      lcd_db4_ddr_high();
      lcd_db3_ddr_high();
      lcd_db2_ddr_high();
      lcd_db1_ddr_high();
      lcd_db0_ddr_high();

      lcd_db7_port_high();                            // Pins High (Inactive)
      lcd_db6_port_high();
      lcd_db5_port_high();
      lcd_db4_port_high();
      lcd_db3_port_high();
      lcd_db2_port_high();
      lcd_db1_port_high();
      lcd_db0_port_high();
    #endif
    
    lcd_rw_port_low();

    #if (WAIT_MODE==0 || RW_LINE_IMPLEMENTED==0)
    if (rs)
      delay_us(40);
    else delay_us(1);
    #endif
    return data;
  }

uint8_t lcd_getc()
  {
    return lcd_read(1);
  }
uint8_t lcd_geta()
{
    return (lcd_read(0));
}
#endif

/*************************************************************************
Low-level function to write byte to LCD controller
Input:    data   byte to write to LCD
          rs     1: write data
                 0: write instruction
Returns:  none
*************************************************************************/
static void lcd_write(uint8_t data,uint8_t rs)
  {
    #if (WAIT_MODE==1 && RW_LINE_IMPLEMENTED==1)
      lcd_waitbusy();
      if (PrevCmdInvolvedAddressCounter)
        {
          delay_us(5);
          PrevCmdInvolvedAddressCounter=0;
        }
    #endif

    if (rs)
      {
        lcd_rs_port_high();                            // RS=1: Write Character
        #if (WAIT_MODE==1 && RW_LINE_IMPLEMENTED==1)
        PrevCmdInvolvedAddressCounter=1;
        #endif
      }
    else
      {
        lcd_rs_port_low();                          // RS=0: Write Command
        #if (WAIT_MODE==1 && RW_LINE_IMPLEMENTED==1)
        PrevCmdInvolvedAddressCounter=0;
        #endif
      }

    #if LCD_BITS==4
      lcd_db7_port_set(data&_BV(7));                  //Output High Nibble
      lcd_db6_port_set(data&_BV(6));
      lcd_db5_port_set(data&_BV(5));
      lcd_db4_port_set(data&_BV(4));

      delay_us(1);
      lcd_e_port_high();

      delay_us(1);
      lcd_e_port_low();

      lcd_db7_port_set(data&_BV(3));                  //Output High Nibble
      lcd_db6_port_set(data&_BV(2));
      lcd_db5_port_set(data&_BV(1));
      lcd_db4_port_set(data&_BV(0));

      delay_us(1);
      lcd_e_port_high();

      delay_us(1);
      lcd_e_port_low();

      lcd_db7_port_high();                            // All Data Pins High (Inactive)
      lcd_db6_port_high();
      lcd_db5_port_high();
      lcd_db4_port_high();

    #else //using 8-Bit_Mode
      lcd_db7_port_set(data&_BV(7));                  //Output High Nibble
      lcd_db6_port_set(data&_BV(6));
      lcd_db5_port_set(data&_BV(5));
      lcd_db4_port_set(data&_BV(4));
      lcd_db3_port_set(data&_BV(3));                  //Output High Nibble
      lcd_db2_port_set(data&_BV(2));
      lcd_db1_port_set(data&_BV(1));
      lcd_db0_port_set(data&_BV(0));

      delay_us(1);
      lcd_e_port_high();
      delay_us(1);
      lcd_e_port_low();

      lcd_db7_port_high();                            // All Data Pins High (Inactive)
      lcd_db6_port_high();
      lcd_db5_port_high();
      lcd_db4_port_high();
      lcd_db3_port_high();
      lcd_db2_port_high();
      lcd_db1_port_high();
      lcd_db0_port_high();
    #endif

    #if (WAIT_MODE==0 || RW_LINE_IMPLEMENTED==0)
      if (!rs && data<=((1<<LCD_CLR) | (1<<LCD_HOME))) // Is command clrscr or home?
        delay_us(1640);
      else delay_us(40);
    #endif
  }

/*************************************************************************
Send LCD controller instruction command
Input:   instruction to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_command(uint8_t cmd)
  {
    #ifndef DONT_USE_LCD
    lcd_write(cmd,0);
    #endif
  }

/*************************************************************************
Set cursor to specified position
Input:    pos position
Returns:  none
*************************************************************************/
void lcd_goto(uint8_t line, uint8_t loc)
{
  #ifndef DONT_USE_LCD
    uint8_t pos;
    switch (line)
    {
    case 1 : pos=(LINE_1_START_ADD+loc); break;
    case 2 : pos=(LINE_2_START_ADD+loc); break;
    case 3 : pos=(LINE_3_START_ADD+loc); break;
    case 4 : pos=(LINE_4_START_ADD+loc); break;        
    default : pos=(LINE_1_START_ADD+loc); line=1; break;
    }
    lcd_command((1<<LCD_DDRAM)+pos);
    g_line = line;
  #endif
}


    
    
/*************************************************************************
Clear screen
Input:    none
Returns:  none
*************************************************************************/
void lcd_clrscr()
  {
    #ifndef DONT_USE_LCD
    lcd_command(1<<LCD_CLR);
    #endif
  }


/*************************************************************************
Return home
Input:    none
Returns:  none
*************************************************************************/
void lcd_home()
  {
    #ifndef DONT_USE_LCD
    lcd_command(1<<LCD_HOME);
    #endif
  }


/*************************************************************************
Display character
Input:    character to be displayed
Returns:  none
*************************************************************************/
void lcd_putc(char c)
  {
    #ifndef DONT_USE_LCD
      char loc; 
      lcd_waitbusy(); //wait for busy flag to disappear
      loc = lcd_read(0);
           
              
      if (c == '\n')
      {
          if (loc >= 0x00 && loc <= 0x13)
              g_line = 1;
          else if (loc >= 0x40 && loc <= 0x53)
              g_line = 2;
          else if (loc >= 0x14 && loc <= 0x27)
              g_line = 3;
          else if (loc >= 0x54 && loc <= 0x67)
              g_line = 4;
          else 
          {
              lcd_home();
              g_line = 1;
          }
          if (g_line > 4)
              g_line = 1;
          
          lcd_goto(g_line+1, 0);
      }
      else
          lcd_write(c,1);
      switch (loc)
      {
      case 0x13 : lcd_goto(2, 0); break;
      case 0x53 : lcd_goto(3, 0); break;
      case 0x27 : lcd_goto(4, 0); break;
      case 0x67 : lcd_goto(1, 0); break;
      }
      #endif
 }

void printlcd(int line, int xpos, char *szFormat, ...)
{
  #ifndef DONT_USE_LCD
    char szBuffer[256]; //in this buffer we form the message
    int NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
    va_list pArgs;
    va_start(pArgs, szFormat);
    vsnprintf(szBuffer, NUMCHARS -1, szFormat, pArgs);
    va_end(pArgs);
    lcd_goto(line, xpos);
    lcd_puts(szBuffer);
  #endif
}



/*************************************************************************
Display string
Input:    string to be displayed
Returns:  none
*************************************************************************/
void lcd_puts(const char *s)
  {
    #ifndef DONT_USE_LCD
    register char c;

    while ((c=*s++))
        lcd_putc(c);
    #endif
  }

 

/*************************************************************************
Display string from flash
Input:    string to be displayed
Returns:  none
*************************************************************************/
void lcd_puts_P(const char *progmem_s)
  {
    #ifndef DONT_USE_LCD
    register char c;

    while ((c=pgm_read_byte(progmem_s++))) 
      lcd_putc(c);
  #endif
  }

/*************************************************************************
Initialize display
Input:    none
Returns:  none
*************************************************************************/
void lcd_init()
  {
    #ifndef DONT_USE_LCD
      //Set All Pins as Output
      lcd_e_ddr_high();
      lcd_rs_ddr_high();
      #if RW_LINE_IMPLEMENTED==1
        lcd_rw_ddr_high();
      #endif
        lcd_db7_ddr_high();
        lcd_db6_ddr_high();
        lcd_db5_ddr_high();
        lcd_db4_ddr_high();
      #if LCD_BITS==8
        lcd_db3_ddr_high();
        lcd_db2_ddr_high();
        lcd_db1_ddr_high();
        lcd_db0_ddr_high();
      #endif

      //Set All Control Lines Low
      lcd_e_port_low();
      lcd_rs_port_low();
      #if RW_LINE_IMPLEMENTED==1
        lcd_rw_port_low();
      #endif

      //Set All Data Lines High
      lcd_db7_port_high();
      lcd_db6_port_high();
      lcd_db5_port_high();
      lcd_db4_port_high();
      #if LCD_BITS==8
        lcd_db3_port_high();
        lcd_db2_port_high();
        lcd_db1_port_high();
        lcd_db0_port_high();
      #endif

      //Startup delay
      delay_ms(100);

      //Initialize Display
      lcd_db7_port_low();
      lcd_db6_port_low();
      delay_us(10);
      lcd_e_port_high();
      delay_us(10);
      lcd_e_port_low();

      delay_us(4100);

      lcd_e_port_high();
      delay_us(10);
      lcd_e_port_low();

      delay_us(100);

      lcd_e_port_high();
      delay_us(1);
      lcd_e_port_low();

      delay_us(40);

      //Init differs between 4-bit and 8-bit from here
      #if (LCD_BITS==4)
        lcd_db4_port_low();
        delay_us(10);
        lcd_e_port_high();
        delay_us(10);
        lcd_e_port_low();
        delay_us(40);

        lcd_db4_port_low();
        delay_us(1);
        lcd_e_port_high();
        delay_us(1);
        lcd_e_port_low();
        delay_us(1);

        #if (LCD_DISPLAYS==1)
          if (LCD_DISPLAY_LINES>1)
            lcd_db7_port_high();
        #else
          unsigned char c;
          switch (ActiveDisplay)
            {
            case 1 : c=LCD_DISPLAY_LINES; break;
            case 2 : c=LCD_DISPLAY2_LINES; break;
  #if (LCD_DISPLAYS>=3)
            case 3 : c=LCD_DISPLAY3_LINES; break;
  #endif
  #if (LCD_DISPLAYS==4)
            case 4 : c=LCD_DISPLAY4_LINES; break;
  #endif
            }
          if (c>1)
            lcd_db7_port_high();
        #endif

        delay_us(1);
        lcd_e_port_high();
        delay_us(1);
        lcd_e_port_low();
        delay_us(40);
      #else
        #if (LCD_DISPLAYS==1)
          if (LCD_DISPLAY_LINES<2)
            lcd_db3_port_low();
        #else
          unsigned char c;
          switch (ActiveDisplay)
            {
              case 1 : c=LCD_DISPLAY_LINES; break;
              case 2 : c=LCD_DISPLAY2_LINES; break;
              #if (LCD_DISPLAYS>=3)
              case 3 : c=LCD_DISPLAY3_LINES; break;
              #endif
              #if (LCD_DISPLAYS==4)
              case 4 : c=LCD_DISPLAY4_LINES; break;
              #endif
            }
          if (c<2)
            lcd_db3_port_low();
        #endif

        lcd_db2_port_low();
        delay_us(1);
        lcd_e_port_high();
        delay_us(1);
        lcd_e_port_low();
        delay_us(40);
      #endif

      //Display Off
      lcd_command(_BV(LCD_DISPLAYMODE));

      //Display Clear
      lcd_clrscr();

      //Entry Mode Set
      lcd_command(_BV(LCD_ENTRY_MODE) | _BV(LCD_ENTRY_INC));

      //Display On
      lcd_command(_BV(LCD_DISPLAYMODE) | _BV(LCD_DISPLAYMODE_ON));
    #endif
  }

#if (LCD_DISPLAYS>1)
void lcd_use_display(int ADisplay)
  {
    if (ADisplay>=1 && ADisplay<=LCD_DISPLAYS)
      ActiveDisplay=ADisplay;
  }
#endif

  unsigned char printlcd_scroll_poll(uint8_t lcdLine, char *text)
  {

    int length = strlen(text);        //length of the passed in string
    int ii = 0;                       //iterative counter
    static int cnt = 0;               //offset counter
    char line[LCD_DISPLAY_CHARS + 1]; //location of string to display
    uint32_t time_now = timer_get();  //time variable
    static uint32_t time_last = 0;    //for remembering the last time

    //initialise our string.
    for (ii = 0; ii < LCD_DISPLAY_CHARS + 1; ii++)
      line[ii] = 0;

    //every 2thirds of a second we will shift right on the array
    if (time_now - time_last > _THIRD_SECONDS_IN_TICKS(2))
    {
      //we only want to print so we fill the line at
      //the end of the string
      if (cnt < length) //-LCD_DISPLAY_CHARS+1)
      {
        strncpy(line, &text[cnt], LCD_DISPLAY_CHARS); //copy our window
                                                      //printlcd_var(1, 0, strlen(line));
        if (strlen(line) < LCD_DISPLAY_CHARS)
        {
          for (ii = 0; ii < (LCD_DISPLAY_CHARS - strlen(line)); ii++)
          {
            strcat(line, " ");
          }
        }
        line[strlen(line)] = '\0';  //add the null terminator
        printlcd(lcdLine, 0, line); //display our text
       
        cnt++;                      //increase our offset counter
      }
      //if the line is too short we print it now

      else

      {

        strncpy(line, &text[0], LCD_DISPLAY_CHARS);
        printlcd(lcdLine, 0, line);
        cnt = 0;              //reset the counter so we continouosly loop
      }
      time_last = time_now; //reset the timer
    }

    return 0;
  }
