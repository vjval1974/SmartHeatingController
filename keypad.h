///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2007, Brad Goold, All Rights Reserved.
//
// Authors: Brad Goold
//
// Date: 19 Mar 2009
//
///////////////////////////////////////////////////////////////////////////////

#ifndef KEYPAD_H
#define KEYPAD_H


#define KEY_ACK    0x0F
#define KEY_HELP   0x0E
#define KEY_MENU   0x0D
#define KEY_ESC   0x0C

#define BUTTON_UP 2
#define BUTTON_DOWN 8
#define BUTTON_LEFT 4
#define BUTTON_RIGHT 6
#define BUTTON_SUBMIT 0x0A
#define BUTTON_CANCEL 0x0B

unsigned char keys_get(void);

#endif
