#ifndef _MAIN_H
#define _MAIN_H

#include <gba_types.h>


extern int display_frame;
extern int input;
extern u16* vid_addr;
extern int debug_val;

// void fill_scanline(int addr, int num, int col);
void plot_pixel(u16* addr, int col);
void fast_clear(u16* vid_addr);
int fastrandom();

#endif
