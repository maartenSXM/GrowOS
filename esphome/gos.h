#ifndef __INCLUDE_GOS_H__
#define __INCLUDE_GOS_H__

#include "config.h"	// from ./bsps/<GOS_BSP>/
#include "configAll.h"	// from ./

// some C preprocess string concatenation macros that are useful
// for passing esphome and platformio some generated build_flags.
// See env.yaml for exmaples of how they can be used.
//
#define GOS_ID(x) x
#define GOS_STRINGIFY_(x)   #x
#define GOS_STRINGIFY(x)    GOS_STRINGIFY_(x)
#define GOS_STRINGIFY4(w,x,y,z)	\
			    GOS_STRINGIFY(GOS_ID(w)GOS_ID(x)GOS_ID(y)GOS_ID(z))
#define GOS_STRINGIFY3(w,x,y)	\
			    GOS_STRINGIFY(GOS_ID(w)GOS_ID(x)GOS_ID(y))
#define GOS_STRINGIFY2(w,x)	\
			    GOS_STRINGIFY(GOS_ID(w)GOS_ID(x))

#endif // __INCLUDE_GOS_H__
