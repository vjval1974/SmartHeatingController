///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2007, Brad Goold, All Rights Reserved.
//
// Authors: Brad Goold
//
// Date: 17 May 2009
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MENUES_H
#define MENUES_H
//-------------------------------------------------------------------------
// Menu Declarations:
// Menu Items are a struct with 4 elements:-
//
//       Element 1 (Char *)      = Pointer to the text that will be 
//                                 displayed on the display
//       Element 2 (menu *)      = Pointer to the Next menu called from 
//                                 activating this one.
//       Element 3 (function *)  = Pointer to the function called by 
//                                 activating this menu function. 
//       Element 4 (key_handler*)= Pointer to the key_handler that handles
//                                 the function called above (if any)
// 
// Note: Menu elements can be left NULL which is like a "do nothing" for 
//       function that it can handle. 
//
// IMPORTANT: The lowest level menu must be declared before the upper 
//            levels in the program flow. ie. if the Main Menu has pointers 
//            to 2 menues, User and Engineer, then these menues must be 
//            declared before main. (and so on) so the lowest level of
//            the menu tree will be declared first. 
//
//            Some menues called from here may be declared in header files
//            which are included. Obviously they are declared before the 
//            calling menus declared below.
//-------------------------------------------------------------------------
#include "menu.h"
//#include "settings.h"
//#include "display.h"
#include "console.h"
//#include "password.h"
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdio.h>





#endif
