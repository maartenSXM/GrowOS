#ifndef __INCLUDE_GOS_H__
#define __INCLUDE_GOS_H__

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

// Configuration files are included intentionally in this order:
//  1. GOS_BSP_PATH/bsp.h which advertises BSP hardware available
//  2. GOS_CONFIG_PATH which defines and configures project features
//  3. bsps/common/configBspAll.h which implements project hardware featues
//  4. projects/configAll.h which implements project software features

#include GOS_STRINGIFY2(GOS_BSP_PATH,/bsp.h)
#include GOS_STRINGIFY(GOS_CONFIG_PATH)

#include "bsps/common/configBspAll.h"
#include "projects/configAll.h"

#endif // __INCLUDE_GOS_H__
