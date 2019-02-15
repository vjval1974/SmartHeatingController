///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2007, Brad Goold, All Rights Reserved.
//
// Authors: Brad Goold
//
// Date: 19 Mar 2009
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MENU_H
#define MENU_H


struct menu
{
    const char *display_text;
    struct menu *next;
    void (*runfunc)(void);
    void (*key_handler)(unsigned char key);
};

void menu_set_root(struct menu *root_menu);
void menu_key(unsigned char key);
void menu_clear(void);
void menu_run_applet(void (*applet_key_handler)(unsigned char key));
void menu_update(void);


#endif
