#ifndef __INCLUDE_GOS_H__
#define __INCLUDE_GOS_H__

// these are defined so they can be used in c preprocessor comparisons
#define GOS_FLASH_SIZE_2MB  2
#define GOS_FLASH_SIZE_4MB  4
#define GOS_FLASH_SIZE_8MB  8
#define GOS_FLASH_SIZE_16MB 16

// some C preprocess string concatenation macros that are useful
// for passing esphome and platformio some generated build_flags.
// See env.yaml for exmaples of how they can be used.
//
#define GOS_X(x)		x
#define GOS_STRINGIFY_(x)	#x
#define GOS_STRINGIFY(x)	GOS_STRINGIFY_(x)
#define GOS_STRINGIFY2(w,x)	GOS_STRINGIFY(GOS_X(w)GOS_X(x))
#define GOS_STRINGIFY3(w,x,y)	GOS_STRINGIFY(GOS_X(w)GOS_X(x)GOS_X(y))
#define GOS_STRINGIFY4(w,x,y,z) GOS_STRINGIFY(GOS_X(w)GOS_X(x)GOS_X(y)GOS_X(z))

#include GOS_STRINGIFY3(bsps/,GOS_BSP,/configBsp.h)
#include "bsps/common/configBspAll.h"
#include "configAll.h"

#endif // __INCLUDE_GOS_H__
