#ifndef _DEFS_H_
#define _DEFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


#ifndef uint8_t
typedef unsigned char uint8_t;
#endif
#ifndef uint16_t
typedef unsigned short uint16_t;
#endif
#ifndef uint32_t
typedef unsigned int uint32_t;
#endif
//TODO
#ifndef _MTK_BUILD
#ifndef _ANDROID_BUILD
#ifndef uint64_t
typedef unsigned long int uint64_t;
#endif
#endif
#endif

//#define __PRETTY_FUNCTION__     __func__


#endif
