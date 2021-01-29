#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <gba_types.h>

// from tonc_types.h
#define IWRAM_DATA __attribute__((section(".iwram")))
#define EWRAM_DATA __attribute__((section(".ewram")))
#define  EWRAM_BSS __attribute__((section(".sbss")))
#define IWRAM_CODE __attribute__((section(".iwram"), long_call))
#define EWRAM_CODE __attribute__((section(".ewram"), long_call))

#define FP_SHIFT 12

#define TO_FP(x) ((x) << FP_SHIFT)
#define TO_INT(x) ((x) >> FP_SHIFT)

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define SIGNUM(x) ((x) > 0 ? 1 : (x) < 0 ? -1 : 0)

typedef unsigned char byte;

#endif
