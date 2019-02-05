//*************************************************************
//*** FUNCTIONS FOR EEPROM READ/WRITE ON I2C COMMUNICATION  ***
//*************************************************************
//Controller:	ATmega128 (Crystal: 16Mhz)
//Compiler:		winAVR (AVR-GCC)
//Author:		CC Dharmani, Chennai (India)
//				www.dharmanitech.com
//Date:			April 2009
//********************************************************

//link to post: http://www.dharmanitech.com/2008/08/interfacing-rtc-serial-eeprom-using-i2c.html
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "RTC_routines.h"
#include "UART_routines.h"
#include "i2c_routines.h"
#include "hd44780.h"
#include "delay.h"
#include "util.h"

char rtc_register[7];
char time[10]; 		//xxam:xx:xx;
char date[12];		//xx/xx/xxxx;
char day;

//***************************************************************************
//Function to set initial address of the RTC for subsequent reading / writing
//***************************************************************************
void RTC_setStartAddress(void)
{
   unsigned char errorStatus;
   
   errorStatus = i2c_start();
   if(errorStatus == 1)
   {
       printf("RTC start1 failed..");
       i2c_stop();
       return;
   } 
   
   errorStatus = i2c_sendAddress(DS1307_W);
   
   if(errorStatus == 1)
   {
     printf("RTC sendAddress1 failed..");
	 i2c_stop();
	 return;
   } 
   
   errorStatus = i2c_sendData(0x00);
   if(errorStatus == 1)
   {
     printf("RTC write-2 failed..");
	 i2c_stop();
	 return;
   } 

   i2c_stop();
}

uint16_t RTC_get_seconds(void){
    RTC_read();
    char x = rtc_register[0];
    static char y;
    static uint16_t ret = 0;
     if (y != x){
            ret++;
            y = x;
        }
     
 return ret;
}
 
//***********************************************************************
//Function to read RTC registers and store them in buffer rtc_register[]
//***********************************************************************    
void RTC_read(void)
{

   
    unsigned char errorStatus, i, data;
    
    errorStatus = i2c_start();
    if(errorStatus == 1)
    {
        printf("RTC start1 failed..");
        i2c_stop();
        return;
    } 
    
    errorStatus = i2c_sendAddress(DS1307_W);
    
    if(errorStatus == 1)
    {
        printf("RTC sendAddress1 failed..");
        i2c_stop();
        return;
    } 
    
    errorStatus = i2c_sendData(0x00);
    if(errorStatus == 1)
    {
        printf("RTC write-1 failed..");
        i2c_stop();
        return;
    } 
    
    errorStatus = i2c_repeatStart();
    if(errorStatus == 1)
    {
        printf("RTC repeat start failed..");
        i2c_stop();
        return;
    } 
    
    errorStatus = i2c_sendAddress(DS1307_R);
    
    if(errorStatus == 1)
    {
        printf("RTC sendAddress2 failed..");
        i2c_stop();
        return;
    } 
    
    for(i=0;i<7;i++)
    {
        if(i == 6)  	 //no Acknowledge after receiving the last byte
            data = i2c_receiveData_NACK();
        else
            data = i2c_receiveData_ACK();
        
   	  if(data == ERROR_CODE)
   	  {
       		printf("RTC receive failed..");
			i2c_stop();
	   		return;
   	  }
	 
	  rtc_register[i] = data;
    }
    
    i2c_stop();
}	  

//******************************************************************
//Function to form time string for sending it to LCD & UART
//****************************************************************** 
void RTC_getTime(void)
{
   RTC_read();
   time[8] = 0x00;	  //NIL
   time[7] = (SECONDS & 0x0f) | 0x30;		//seconds(1's)
   time[6] = ((SECONDS & 0x70) >> 4) | 0x30;		//seconds(10's)
   time[5] = ':';
   
   time[4] = (MINUTES & 0x0f) | 0x30;
   time[3] = ((MINUTES & 0x70) >> 4) | 0x30;
   time[2] = ':'; 
   
   time[1] = (HOURS & 0x0f) | 0x30;	
   time[0] = ((HOURS & 0x30) >> 4) | 0x30;
 
}

//******************************************************************
//Function to form date string for sending it to LCD & UART
//****************************************************************** 
void RTC_getDate(void)
{
  RTC_read();
  date[11] = 0x00;
  date[10] = 0x00;
  date[9] = (YEAR & 0x0f) | 0x30;
  date[8] = ((YEAR & 0xf0) >> 4) | 0x30;
  date[7] = '0';
  date[6] = '2';
  date[5] = '/';
  date[4] = (MONTH & 0x0f) | 0x30;
  date[3] = ((MONTH & 0x10) >> 4) | 0x30;
  date[2] = '/';
  date[1] = (DATE & 0x0f) | 0x30;
  date[0] = ((DATE & 0x30) >> 4) | 0x30;
}  
  
//******************************************************************
//Function to display time on LCD and send it to PC (thru UART)
//****************************************************************** 
void RTC_displayTime()
{
  RTC_getTime();
  printf("Time:"); 
  printf(time);
  printf("\r\n"); 
//printlcd(x, y, "%s", time);     
}

char * RTC_GetTimeString()
{
    RTC_getTime();
    return time;
}

//******************************************************************
//Function to display date on LCD and send it to PC (UART)
//****************************************************************** 
void RTC_displayDate(void)
{
  RTC_getDate();
  printf(PSTR("Date:")); 
  printf(date); 
  printf("\r\n"); 
  RTC_displayDay();   
}

//******************************************************************
//Function to get the string for day 
//****************************************************************** 
void RTC_displayDay(void)
{
    RTC_getDate();
    RTC_updateRegisters();
    printf("Day: ");
  
  switch(DAY)
  {
   case 0:printf("Sunday");
          break; 
   case 1:printf("Monday");
          break; 
   case 2:printf("Tuesday");
          break; 
   case 3:printf("Wednesday");
          break; 
   case 4:printf("Thursday");
          break; 		  
   case 5:printf("Friday");
          break; 		  
   case 6:printf("Saturday");
          break; 
   default:	printf("Unknown");  
  }
  printf("\r\n");
}	  		  
		  		     	  
//******************************************************************
//Function to update buffer rtc_register[] for next writing to RTC
//****************************************************************** 
void RTC_updateRegisters(void)
{
  SECONDS = ((time[6] & 0x07) << 4) | (time[7] & 0x0f);
  MINUTES = ((time[3] & 0x07) << 4) | (time[4] & 0x0f);
  HOURS = ((time[0] & 0x03) << 4) | (time[1] & 0x0f);  
  DAY = date[10];
  DATE = ((date[0] & 0x03) << 4) | (date[1] & 0x0f);
  MONTH = ((date[3] & 0x01) << 4) | (date[4] & 0x0f);
  YEAR = ((date[8] & 0x0f) << 4) | (date[9] & 0x0f);
}  


//******************************************************************
//Function to write new time in the RTC 
//******************************************************************   
unsigned char RTC_writeTime(void)
{
  unsigned char errorStatus, i;
  
   errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     printf("RTC start1 failed..");
   	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendAddress(DS1307_W);
   
   if(errorStatus == 1)
   {
     printf("RTC sendAddress1 failed..");
	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendData(0x00);
   if(errorStatus == 1)
   {
     printf(PSTR("RTC write-1 failed.."));
	 i2c_stop();
	 return(1);
   } 

    for(i=0;i<3;i++)
    {
	  errorStatus = i2c_sendData(rtc_register[i]);  
   	  if(errorStatus == 1)
   	  {
       		printf("RTC write time failed..");
			i2c_stop();
	   		return(1);
   	  }
    }
	
	i2c_stop();
	return(0);
}


//******************************************************************
//Function to write new date in the RTC
//******************************************************************   
unsigned char RTC_writeDate(void)
{
  unsigned char errorStatus, i;
  
   errorStatus = i2c_start();
   if(errorStatus == 1)
   {
     printf("RTC start1 failed..");
   	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendAddress(DS1307_W);
   
   if(errorStatus == 1)
   {
     printf("RTC sendAddress1 failed..");
	 i2c_stop();
	 return(1);
   } 
   
   errorStatus = i2c_sendData(0x03);
   if(errorStatus == 1)
   {
     printf("RTC write-1 failed..");
	 i2c_stop();
	 return(1);
   } 

    for(i=3;i<7;i++)
    {
	  errorStatus = i2c_sendData(rtc_register[i]);  
   	  if(errorStatus == 1)
   	  {
       		printf("RTC write date failed..");
			i2c_stop();
	   		return(1);
   	  }
    }
	
	i2c_stop();
	return(0);
}
  
//******************************************************************
//Function to update RTC time by entering it at hyper terminal
//******************************************************************   
void RTC_updateTime(void)
{
    char data[8];
    char datas[10];
    printf("Enter Time in 24h format(hh:mm:ss):"); 
    
    //printf(data); 
    int ii;
    for (ii=0; ii < 8; ii++)
        data[ii] = 0;
    
    while (data[2] == 0)
    {
        uart_get_last_line(data);
        delay_ms(100);
    }   
    

    //printlcd(3, 0, data);
    delay_ms(100);
    if(data[0] < 0x30 || data[0] > 0x32)
        goto TIME_ERROR;
	   
    time[0]= data[0];
    
    
    if(data[1] < 0x30 || data[1] > 0x39)
        goto TIME_ERROR;
	 
	time[1]= data[1];
	
		
	if(((time[1] & 0x0f) + ((time[0] & 0x03)*10)) > 23)
	   goto TIME_ERROR;
	 
	//minutes
	
	if(data[3] < 0x30 || data[3] > 0x35)
	   goto TIME_ERROR;
	   
	time[3]= data[3]; 
	
	
	if(data[4] < 0x30 || data[4] > 0x39)
	   goto TIME_ERROR;
	   
	time[4]= data[4]; 
	

	if(data[6] < 0x30 || data[6] > 0x35)
	   goto TIME_ERROR;
	   
	time[6]= data[6]; 
	
	if(data[7] < 0x30 || data[7] > 0x39)
	   goto TIME_ERROR;
	   
	time[7]= data[7]; 
	
	 	  
  RTC_updateRegisters(); 
  if (RTC_writeTime() == 0);
  

  {
    
  	 printf("Time Updated sucessfully..\\A"); 
  }	  
  return;
  
TIME_ERROR:

  
  printf("Invalid Entry.. %s ", datas); 
  return;
}  
  
  
//******************************************************************
//Function to update RTC date by entering it at hyper terminal
//******************************************************************   
void RTC_updateDate(void)
{
  char data[8];
  int i;
  for (i = 0; i < 8; i++)
      data[i] = 0;

  printf("Enter Date (dd/mm/yy):"); 
  while (data[2] == 0)
  {
      uart_get_last_line(data); 
  }

  if(data[0] < 0x30 || data[0] > 0x33)
      goto DATE_ERROR;
  
  date[0]= data[0];
  

  if(data[1] < 0x30 || data[1] > 0x39)
      goto DATE_ERROR;
  
  date[1]= data[1];
  
  if(((time[8] & 0x0f) + ((time[9] & 0x03)*10)) > 31)
      goto DATE_ERROR;
  
  //receive special char (/,-, etc.)
  
  if(data[2] != '/' && data[2] != '-' && data[2] != ' ')
      goto DATE_ERROR;
  date[2] = data[2];
  
  
  //receive month
  
  if(data[3] < 0x30 || data[3] > 0x31)
      goto DATE_ERROR;
  
  date[3]= data[3]; 
  
  
  if(data[4] < 0x30 || data[4] > 0x39)
      goto DATE_ERROR;
  
  date[4] = data[4]; 
  
  if(((date[4] & 0x0f) + ((date[3] & 0x03)*10)) > 12)
      goto DATE_ERROR;
  
  //receive special char (/,-, etc.)
  
  if(data[5] != '/' && data[5] != '-' && data[5] != ' ')
      goto DATE_ERROR;
  date[5] = data[5];
  
  date[6] = '2'; 	   	   	  	  //year is 20xx
  date[7] = '0';
  
  
  if(data[6] < 0x30 || data[6] > 0x39)
      goto DATE_ERROR;
  
  date[8]= data[6]; 
  
  
  if(data[7] < 0x30 || data[7] > 0x39)
      goto DATE_ERROR;
  
  date[9]= data[7]; 
  
  
  printf("Enter Day (Sunday:0, Monday:1...) (0-6):"); 
  for (i = 0; i < 8; i++)
      data[i] = 0;

  while (data[0] == 0)
  {
      uart_get_last_line(data);
  }

  if(data[0] < 0x30 || data[0] > 0x36)
      goto DATE_ERROR;
  
  date[10] = data[0] & 0x0f;
  
  RTC_updateRegisters(); 
  if (RTC_writeDate()==0)
      
  {
      printf("Date Updated sucessfully.."); 
  }	  
  return;
  
  DATE_ERROR:

  
  printf("Invalid Entry.."); 
  return;
}  
  
  
	
	  
 
 

 
