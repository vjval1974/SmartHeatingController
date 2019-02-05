///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2007, Matthew Pratt
//
// Licensed under the GNU General Public License v3 or greater
//
// Date: 10 Jun 2007
///////////////////////////////////////////////////////////////////////////////

#ifndef UTIL_H
#define UTIL_H

#define GLUE(a, b)     a##b
#define PORT(x)        GLUE(PORT, x)
#define PIN(x)         GLUE(PIN, x)
#define DDR(x)         GLUE(DDR, x)


#endif
