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
unsigned char keys_get(void);

#endif
