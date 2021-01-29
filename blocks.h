#ifndef _BLOCKS_H
#define _BLOCKS_H

#define DIR_ZM 0
#define DIR_XP 1
#define DIR_ZP 2
#define DIR_XM 3
#define DIR_YP 4
#define DIR_YM 5

#include "global.h"

//extern const int DIR_OPPOSITE[6];

#define BMAP_X 8
#define BMAP_Y 8
#define BMAP_Z 8

#define BLOCK_INDEX(v) ((v).x + BMAP_X * ((v).y + (v).z * BMAP_Y))
#define BLOCK_INDEX_XYZ(x, y, z) ((x) + BMAP_X * ((y) + (z) * BMAP_Y))
extern byte blockmap[];
extern bool freeze_traversal;

void init_blockmap();
void init_palette();

void traverse_blocks();
void draw_blocks();

#endif