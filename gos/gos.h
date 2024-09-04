#ifndef __INCLUDE_GOS_H__
#define __INCLUDE_GOS_H__

// some C preprocess string concatenation macros that are useful
// for passing esphome and platformio some generated build_flags.
// See below and env.yaml for examples of how they can be used.

#define GOS_X(x)		x
#define GOS_STRINGIFY_(x)	#x
#define GOS_STRINGIFY(x)	GOS_STRINGIFY_(x)
#define GOS_STRINGIFY2(w,x)	GOS_STRINGIFY(GOS_X(w)GOS_X(x))
#define GOS_STRINGIFY3(w,x,y)	GOS_STRINGIFY(GOS_X(w)GOS_X(x)GOS_X(y))
#define GOS_STRINGIFY4(w,x,y,z) GOS_STRINGIFY(GOS_X(w)GOS_X(x)GOS_X(y)GOS_X(z))

// The BSP and project config .h files are intentionally included
// in that order so that the BSP can advertise hardware features
// to the project. They also are included before all GOS yaml files
// so that they have opportunity to configure them. GOS yaml files
// that have configuration features  must declare a default using
// #default so that project config .h files only have to #define
// non-default values. Defaults declared with #default are not
// allowed to be changed since projects are allowed to assume the
// default values. The "print-defaults" make target prints out
// project defaults.

#include GOS_STRINGIFY2(GOS_BSP_DIR,/bsp.h)	// declare BSP hardware
#include GOS_STRINGIFY(GOS_CONFIG_FILE)		// set project configuration

#endif // __INCLUDE_GOS_H__
