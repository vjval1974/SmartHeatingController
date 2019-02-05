///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2007, Matthew Pratt
//
// Licensed under the GNU General Public License v3 or greater
//
// Date: 23 Jun 2007
///////////////////////////////////////////////////////////////////////////////

#ifndef ERR_H
#define ERR_H

typedef unsigned char err_t;

enum Error {
    ERR_NONE = 0,
    ERR_MOTOR_TIMEOUT,
    ERR_OWB_SHORT_CIRCUIT,
    ERR_OWB_NO_PRESSENCE,
    ERR_INDEX_OUT_OF_BOUNDS,
};

#endif
