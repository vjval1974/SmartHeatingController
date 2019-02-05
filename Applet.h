
#ifndef _Applet_h_
#define _Applet_h_

#include <inttypes.h>
#include <avr/io.h>

struct Applet;
typedef struct Applet Applet;


void Display(); 
void SetLine(int num, char *szFormat, ...);
void SetLineIn(int num,  Applet ** ap, char *szFormat, ...);
void InitApplet(Applet ** ap);
void SetApplet(Applet ** newApplet);
void ForgetApplet();
char * GetCurrentAppletName();

#endif