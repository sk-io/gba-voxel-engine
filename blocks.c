#include "blocks.h"

#include <gba_video.h>

#include "main.h"
#include "global.h"
#include "emath.h"
#include "draw.h"

static const int DIR_OPPOSITE[6] = {
    DIR_ZP, DIR_XM, DIR_ZM,
    DIR_XP, DIR_YM, DIR_YP
};

static const vec3_t cube_verts[8] = {
    {TO_FP(0), TO_FP(0), TO_FP(0)},
    {TO_FP(1), TO_FP(0), TO_FP(0)},
    {TO_FP(1), TO_FP(1), TO_FP(0)},
    {TO_FP(0), TO_FP(1), TO_FP(0)},

    {TO_FP(0), TO_FP(0), TO_FP(1)},
    {TO_FP(1), TO_FP(0), TO_FP(1)},
    {TO_FP(1), TO_FP(1), TO_FP(1)},
    {TO_FP(0), TO_FP(1), TO_FP(1)}
};

static const int cube_quads[6][4] = {
    {0, 1, 2, 3},
    {1, 5, 6, 2}, 
    {5, 4, 7, 6},
    {4, 0, 3, 7},
    {3, 2, 6, 7},
    {5, 4, 0, 1}
};

static const int quad_tris[] = {
    0, 1, 2, 2, 3, 0
};

// l, d, l, d, b, t
static const byte block_color1[3][6] = {
    {0}, 
    {8, 6, 8, 6, 10, 4},
    {16, 14, 16, 14, 18, 12},
};

typedef struct {
    blockpos_t pos;
    byte dir;
} facepos_t;

// obviously po2
#define RENDERLIST_SIZE 256
static int renderlist_count;
static facepos_t renderlist[RENDERLIST_SIZE];

EWRAM_BSS byte blockmap[BMAP_X * BMAP_Y * BMAP_Z];
static byte visited[BMAP_X * BMAP_Y * BMAP_Z]; // todo: bitfield

bool freeze_traversal = 0;
static blockpos_t cam_block_pos;

// obviously po2
#define VQUEUE_CAP 256
static blockpos_t vqueue[VQUEUE_CAP];
static short vqueue_start, vqueue_end, vqueue_count;
static int num_blocks_traversed;
int block_traversal_limit;

static inline void set_block(blockpos_t p, byte b) {
    blockmap[BLOCK_INDEX(p)] = 1;
}

void init_blockmap() {
    for (int i = 0; i < BMAP_X * BMAP_Y * BMAP_Z; i++)
        blockmap[i] = 0;

    for (int z = 0; z < BMAP_Z; z++) {
        for (int y = 0; y < BMAP_Y; y++) {
            for (int x = 0; x < BMAP_X; x++) {
                byte b = 0;
                if (x == 0 || y == 0 || z == 0)
                    b = 1;
                if (x == BMAP_X - 1 || y == BMAP_Y - 1 || z == BMAP_Z - 1)
                    b = 1;
                
                if ((fastrandom() & 0b1111) == 0)
                    b = 2;
                
                blockmap[BLOCK_INDEX_XYZ(x, y, z)] = b;
            }
        }
    }
    
    //blockpos_t p = {1, 1, 2};
    //blockmap[BLOCK_INDEX(p)] = 1;

    //for (int i = 0; i < BMAP_X * BMAP_Y * BMAP_Z; i++)
    //    blockmap[i] = fastrandom() & 1;
}

void init_palette() {
    // -- stone
    *(BG_PALETTE + 4) = RGB5(23, 23, 23); // stone top1
    *(BG_PALETTE + 5) = RGB5(22, 22, 22); // stone top2

    *(BG_PALETTE + 6) = RGB5(19, 19, 19); // stone lightside1
    *(BG_PALETTE + 7) = RGB5(18, 18, 18); // stone lightside2

    *(BG_PALETTE + 8) = RGB5(15, 15, 15); // stone darkside1
    *(BG_PALETTE + 9) = RGB5(14, 14, 14); // stone darkside2

    *(BG_PALETTE + 10) = RGB5(11, 11, 11); // stone bottom1
    *(BG_PALETTE + 11) = RGB5(10, 10, 10); // stone bottom2

    // -- dirt
    *(BG_PALETTE + 12) = RGB5(15, 9, 7); // dirt top1
    *(BG_PALETTE + 13) = RGB5(14, 8, 6); // dirt top2

    *(BG_PALETTE + 14) = RGB5(13, 7, 5); // dirt lightside1
    *(BG_PALETTE + 15) = RGB5(12, 6, 4); // dirt lightside2

    *(BG_PALETTE + 16) = RGB5(11, 5, 4); // dirt darkside1
    *(BG_PALETTE + 17) = RGB5(10, 4, 4); // dirt darkside2

    *(BG_PALETTE + 18) = RGB5(8, 3, 2); // dirt bottom1
    *(BG_PALETTE + 19) = RGB5(7, 2, 2); // dirt bottom2
}

static IWRAM_CODE void draw_face(const vec3_t* offset, const int face, const byte block) {
    vec2_t p[4];

    for (int i = 0; i < 4; i++) {
        vec3_t v = cube_verts[cube_quads[face][i]];
        vec_add(&v, &v, offset);

        transform(&v, &v);

        if (v.z <= 2048)
            return;
        if (v.z >= 0xFFFF)
            return;
        
        persp_div(p + i, &v);
    }

    byte col1 = block_color1[block][face];
    byte col2 = col1 + 1;

    sort_and_fill_tri(&p[quad_tris[0]], &p[quad_tris[1]], &p[quad_tris[2]], col1);
    sort_and_fill_tri(&p[quad_tris[3]], &p[quad_tris[4]], &p[quad_tris[5]], col2);
}

inline IWRAM_CODE static void vqueue_clear() {
    vqueue_count = vqueue_start = vqueue_end = 0;
}

inline IWRAM_CODE static void vqueue_enque(const blockpos_t* v) {
    if (vqueue_count == VQUEUE_CAP - 1) {
        plot_pixel(vid_addr + 120 * 15, 3);
        return;
    }
    vqueue[vqueue_end++] = *v;
    vqueue_end &= VQUEUE_CAP - 1;
    vqueue_count++;
}

inline IWRAM_CODE static void vqueue_deque() {
    vqueue_start++;
    vqueue_start &= VQUEUE_CAP - 1;
    vqueue_count--;
}

IWRAM_CODE inline static bool is_in_bounds(const blockpos_t* pos) {
    if (pos->x < 0) return false;
    if (pos->y < 0) return false;
    if (pos->z < 0) return false;
    if (pos->x >= BMAP_X) return false;
    if (pos->y >= BMAP_Y) return false;
    if (pos->z >= BMAP_Z) return false;
    return true;
}

// backface culling is fast cause all the faces are axis aligned
IWRAM_CODE inline static bool is_face_visible(const blockpos_t* pos, const int dir) {
    if (dir == 0) if (cam_block_pos.z >= pos->z) return false;
    if (dir == 1) if (cam_block_pos.x <= pos->x) return false;
    if (dir == 2) if (cam_block_pos.z <= pos->z) return false;
    if (dir == 3) if (cam_block_pos.x >= pos->x) return false;
    if (dir == 4) if (cam_block_pos.y <= pos->y) return false;
    if (dir == 5) if (cam_block_pos.y >= pos->y) return false;
    return true;
}

IWRAM_CODE inline static void increment_pos(blockpos_t* pos, const int dir) {
    switch (dir) {
    case DIR_ZM:
        pos->z--;
        break;
    case DIR_XP:
        pos->x++;
        break;
    case DIR_ZP:
        pos->z++;
        break;
    case DIR_XM:
        pos->x--;
        break;
    case DIR_YP:
        pos->y++;
        break;
    case DIR_YM:
        pos->y--;
        break;
    }
}

IWRAM_CODE static void check_adj(blockpos_t* origin, int dir) {
    blockpos_t pos = *origin;
    increment_pos(&pos, dir);

    if (!is_in_bounds(&pos))
        return;
    
    if (0) {
        // prevent going backwards, doesn't work
        blockpos_t vdir = {0, 0, 0};
        increment_pos(&vdir, dir);

        vec3_t v1 = { TO_FP(vdir.x), TO_FP(vdir.y), TO_FP(vdir.z) };

        if (vec_dot(&v1, &cam_forward) < -3000)
            return;
    }

    if (num_blocks_traversed > 5) {
        // jank frustum culling: just project the centers of the blocks and see if they're on screen.
        // cheaper than creating an actual frustum and whatnot
        vec3_t center = {
            TO_FP(pos.x) + 2048,
            TO_FP(pos.y) + 2048,
            TO_FP(pos.z) + 2048
        };

        transform(&center, &center);

        if (center.z < 1000)
            return;
        
        vec2_t scr;
        persp_div(&scr, &center);

        const int margin = TO_FP(80);
        if (scr.x < -margin)
            return;
        if (scr.y < -margin)
            return;
        if (scr.x > TO_FP(SCREEN_WIDTH) + margin)
            return;
        if (scr.y > TO_FP(SCREEN_HEIGHT) + margin)
            return;
    }

    int index = BLOCK_INDEX(pos);

    byte b = blockmap[index];
    if (b != 0) {
        if (!is_face_visible(&pos, DIR_OPPOSITE[dir]))
            return;
        
        if (renderlist_count < RENDERLIST_SIZE) {
            renderlist[renderlist_count].pos = pos;
            renderlist[renderlist_count].dir = DIR_OPPOSITE[dir];
            renderlist_count++;
        }
    } else if (visited[index] == 0) {
        vqueue_enque(&pos);
        visited[index] = 1;
    }
}

IWRAM_CODE void traverse_blocks() {
    if (freeze_traversal)
        return;

    for (int i = 0; i < BMAP_X * BMAP_Y * BMAP_Z; i++)
        visited[i] = 0;
    renderlist_count = 0;
    vqueue_clear();
    num_blocks_traversed = 0;

    cam_block_pos.x = cam_pos.x >> 12;
    cam_block_pos.y = cam_pos.y >> 12;
    cam_block_pos.z = cam_pos.z >> 12;

    if (!is_in_bounds(&cam_block_pos))
        return;
    
    // plot_pixel(vid_addr + 120 * 3 + (cam_block_pos.x), 3);
    // plot_pixel(vid_addr + 120 * 4 + (cam_block_pos.y), 3);
    // plot_pixel(vid_addr + 120 * 5 + (cam_block_pos.z), 3);

    visited[BLOCK_INDEX(cam_block_pos)] = 1;
    vqueue_enque(&cam_block_pos);

    while (vqueue_count != 0) {
        blockpos_t* p = &vqueue[vqueue_start];
        vqueue_deque();

        check_adj(p, 0);
        check_adj(p, 1);
        check_adj(p, 2);
        check_adj(p, 3);
        check_adj(p, 4);
        check_adj(p, 5);

        if (num_blocks_traversed++ == block_traversal_limit) return;
        if (renderlist_count == RENDERLIST_SIZE - 1) return;
    }
}

IWRAM_CODE void draw_blocks() {
    // draw faces back to front
    for (int i = renderlist_count - 1; i >= 0; i--) {
        const facepos_t* t = renderlist + i;
        vec3_t p = {TO_FP(t->pos.x), TO_FP(t->pos.y), TO_FP(t->pos.z)};
        byte block = blockmap[BLOCK_INDEX(t->pos)];
        draw_face(&p, t->dir, block);
    }
}
