#ifndef __INCLUDE_GOS_H__
#define __INCLUDE_GOS_H__

#include "config.h"	// from ./bsps/<GOS_BSP>/
#include "configAll.h"	// from ./

// some C preprocess string concatenation macros that are useful
// for passing esphome and platformio some generated build_flags.
// See env.yaml for exmaples of how they can be used.
//
#define _ID(x) x
#define _STR(x) #x
#define STR(x) _STR(x)
#define DEF_CAT4(w,x,y,z) STR(_ID(w)_ID(x)_ID(y)_ID(z))
#define DEF_CAT3(w,x,y)   STR(_ID(w)_ID(x)_ID(y))
#define DEF_CAT2(w,x)     STR(_ID(w)_ID(x))
#define DEF_CAT(w,x)      STR(_ID(w)_ID(x))
#define DEF_NAME(w,x,y,z) _ID(w)_ID(x)_ID(y)_ID(z)

#if GOS_USER_maarten
#define _SERIALNO 100
#else // GOS_USER_erik
#define _SERIALNO 101
#endif

#define GOS_NAME DEF_NAME(gb,_SERIALNO,cpu,GOS_PROJTAG)
#define GOS_NAME_STR STR(GOS_NAME)

#endif // __INCLUDE_GOS_H__
