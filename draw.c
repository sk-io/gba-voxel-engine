#include "draw.h"

#include "main.h"
#include "lut.h"
#include "emath.h"
#include "global.h"

#include <gba_types.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

typedef unsigned int uint;

static IWRAM_CODE inline void fill_scanline(int addr, int num, u16 fill) {
    for (int i = 0; i < num; i++) {
        *(((short*) addr) + i) = fill;
    }
}

static IWRAM_CODE void fill_tri(int x0, int y0, int x1, int y1, int x2, int y2, int col) {
    int dy = TO_INT(y2 - y0);
    if (dy == 0)
        return;
    dy++;
    
    int left_acc = x0;
    int right_acc = x0;

    int slopes[3];

    int flat_top = y1 - y0 == 0;
    int flat_bot = y1 - y2 == 0;

    const int div_shift = 8;

    slopes[0] = fastdiv((x2 - x0) >> 4, (y2 - y0) >> 4);
    if (flat_top) {
        if (x0 < x1) {
            left_acc = x0;
            right_acc = x1;
        } else {
            left_acc = x1;
            right_acc = x0;
        }
    } else {
        slopes[1] = fastdiv((x1 - x0) >> 4, (y1 - y0) >> 4);
    }

    if (!flat_bot) {
        slopes[2] = fastdiv((x2 - x1) >> 4, (y2 - y1) >> 4);
    }

    int left_side = flat_top ? 0 : (slopes[0] > slopes[1]);

    int mid_y = (y1 - y0) >> FP_SHIFT;
    for (int y = 0; y < dy; y++) {
        //plot_pixel(vid_addr + ((left_acc >> 12) >> 1) + (y + (y0 >> FP_SHIFT)) * 120, col);
        //plot_pixel(vid_addr + ((right_acc >> 12) >> 1) + (y + (y0 >> FP_SHIFT)) * 120, col);
        int ty = (y + (y0 >> FP_SHIFT));
        if (!(ty < 0 || ty >= 160)) {
            ty *= 240;

            int lx0 = (left_acc >> FP_SHIFT) & ~0b1;
            int lx1 = (right_acc >> FP_SHIFT) & ~0b1;
            lx1 += 2;
            
            if (lx0 < 0)
                lx0 = 0;
            if (lx1 > 239)
                lx1 = 239;

            fill_scanline(((int)vid_addr + ty + lx0), (lx1 - lx0) >> 1, col | (col << 8));
        }

        if (y == mid_y) {
            slopes[1] = slopes[2];

            if (left_side) 
                left_acc = x1;
            else
                right_acc = x1;
        }

        left_acc += slopes[left_side];
        right_acc += slopes[left_side ^ 1];
    }
}

static IWRAM_CODE inline void swap(vec2_t **x, vec2_t **y) {
    vec2_t *tmp = *x;
    *x = *y;
    *y = tmp;
}

IWRAM_CODE void sort_and_fill_tri(vec2_t *v0, vec2_t *v1, vec2_t *v2, int col) {
    if (v0->y > v2->y)
        swap(&v0, &v2);
    if (v0->y > v1->y)
        swap(&v0, &v1);
    if (v1->y > v2->y)
        swap(&v1, &v2);
    
    if (ABS(v0->y - v1->y) < 4096 * 2) {
        v0->y = v1->y;
        if (v0->x > v1->x) {
            swap(&v0, &v1);
        }
    } else {
        const int extra = 4096;
        v0->y -= extra;
        v2->y += extra;
    }

    if (ABS(v1->y - v2->y) < 4096 * 2) {
        v1->y = v2->y;
        if (v1->x > v2->x) {
            swap(&v1, &v2);
        }
    }

    fill_tri(
        v0->x, v0->y,
        v1->x, v1->y,
        v2->x, v2->y, col);

    // plot_pixel(vid_addr + ((x[p0] >> 12) >> 1) + (y[p0] >> 12) * 120, 1);
    // plot_pixel(vid_addr + ((x[p1] >> 12) >> 1) + (y[p1] >> 12) * 120, 2);
    // plot_pixel(vid_addr + ((x[p2] >> 12) >> 1) + (y[p2] >> 12) * 120, 3);
}

// void draw() {
//     int x[3] = {
//         50 << FP_SHIFT, 20 << FP_SHIFT, 100 << FP_SHIFT
//     };
//     int y[3] = {
//         20 << FP_SHIFT, 45 << FP_SHIFT, 60 << FP_SHIFT
//     };
//     sort_and_fill_tri(x, y, display_frame + 1);
// }
