// THIS FILE IS DEPRECATED

#ifndef AXIUM_SYSTEMCONFIG_H
#define AXIUM_SYSTEMCONFIG_H

#include "Define.h"
#include "revision.h"

#define _PACKAGENAME "AxiumCore"

#if AXIUM_ENDIAN == AXIUM_BIGENDIAN
# define _ENDIAN_STRING "big-endian"
#else
# define _ENDIAN_STRING "little-endian"
#endif

#endif