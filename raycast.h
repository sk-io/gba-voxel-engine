#ifndef _RAYCAST_H
#define _RAYCAST_H

#include "global.h"
#include "emath.h"

typedef struct {
    bool found;
    blockpos_t pos;
    int face;
} raycast_result_t;

extern raycast_result_t raycast_result;

void raycast();

#endif
