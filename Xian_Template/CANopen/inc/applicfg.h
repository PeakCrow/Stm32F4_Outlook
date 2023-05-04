/*
This file is part of CanFestival, a library implementing CanOpen Stack.

Copyright (C): Edouard TISSERANT and Francis DUPIN
AT91 Port: Peter CHRISTEN

See COPYING file for copyrights details.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __APPLICFG_H__
#define __APPLICFG_H__

#include <string.h>
#include <stdio.h>

// Integers
#define INTEGER8 signed char
#define INTEGER16 short
#define INTEGER24 long
#define INTEGER32 long
#define INTEGER40 long long
#define INTEGER48 long long
#define INTEGER56 long long
#define INTEGER64 long long

// Unsigned integers
#define UNS8   unsigned char
#define UNS16  unsigned short
#define UNS32  unsigned long
#define UNS24  unsigned long
#define UNS40  unsigned long long
#define UNS48  unsigned long long
#define UNS56  unsigned long long
#define UNS64  unsigned long long

// Reals
#define REAL32 float
#define REAL64 double

// Reals
#define REAL32	float
#define REAL64 double

#include "can.h"

//#define CANFESTIVAL_DEBUG_MSG(num, str, val)


#define CANFESTIVAL_DEBUG_MSG(num, str, val)\
  {unsigned long value = val;\
   printf("%s(%d) : 0x%X %s 0x%lX\r\n",__FILE__, __LINE__,num, str, value); \
  }

/* Definition of MSG_ERR */
#define MSG_ERR(num, str, val) CANFESTIVAL_DEBUG_MSG(num, str, val)

/* Definition of MSG_WAR */
// ---------------------
#define MSG_WAR(num, str, val) CANFESTIVAL_DEBUG_MSG(num, str, val)

typedef void* CAN_HANDLE;

typedef void* CAN_PORT;

#endif


