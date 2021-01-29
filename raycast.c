#include "raycast.h"
#include "player.h"
#include "blocks.h"
#include "lut.h"

raycast_result_t raycast_result;

inline static bool is_in_bounds(const blockpos_t* pos) {
    if (pos->x < 0) return false;
    if (pos->y < 0) return false;
    if (pos->z < 0) return false;
    if (pos->x >= BMAP_X) return false;
    if (pos->y >= BMAP_Y) return false;
    if (pos->z >= BMAP_Z) return false;
    return true;
}

static int intbound(int s, int ds) {
    if (ds < 0) {
        s = -s;
        ds = -ds;
    }

    if (ds < 256) return 0xFFFF;

    s &= TO_FP(1) - 1;

    const int shift = 2;
    return fastdiv((TO_FP(1) - s) << shift, ds << shift);
}

// from https://gamedev.stackexchange.com/a/49423
void raycast() {
    blockpos_t bpos = {
        TO_INT(cam_pos.x),
        TO_INT(cam_pos.y),
        TO_INT(cam_pos.z)
    };

    int step_x = SIGNUM(cam_forward.x);
    int step_y = SIGNUM(cam_forward.y);
    int step_z = SIGNUM(cam_forward.z);

    int t_max_x = intbound(cam_pos.x, cam_forward.x);
    int t_max_y = intbound(cam_pos.y, cam_forward.y);
    int t_max_z = intbound(cam_pos.z, cam_forward.z);

    const int shift = 2;
    const int min = 128;
    int t_dx = (cam_forward.x > -min && cam_forward.x < min) ? 0xFFFF : fastdiv(TO_FP(1) << shift, ABS(cam_forward.x) << shift);
    int t_dy = (cam_forward.y > -min && cam_forward.y < min) ? 0xFFFF : fastdiv(TO_FP(1) << shift, ABS(cam_forward.y) << shift);
    int t_dz = (cam_forward.z > -min && cam_forward.z < min) ? 0xFFFF : fastdiv(TO_FP(1) << shift, ABS(cam_forward.z) << shift);

    raycast_result.found = false;

    int fuck = 0;
    while (is_in_bounds(&bpos)) {
        if (blockmap[BLOCK_INDEX(bpos)] != 0) {
            raycast_result.found = true;
            raycast_result.pos = bpos;
            return;
        }

        if (t_max_x < t_max_y) {
            if (t_max_x < t_max_z) {
                bpos.x += step_x;
                t_max_x += t_dx;

                raycast_result.face = -step_x > 0 ? DIR_XP : DIR_XM;
            } else {
                bpos.z += step_z;
                t_max_z += t_dz;

                raycast_result.face = -step_z > 0 ? DIR_ZP : DIR_ZM;
                }
            } else {
                if (t_max_y < t_max_z) {
                bpos.y += step_y;
                t_max_y += t_dy;

                raycast_result.face = -step_y > 0 ? DIR_YP : DIR_YM;
            } else {
                bpos.z += step_z;
                t_max_z += t_dz;

                raycast_result.face = -step_z > 0 ? DIR_ZP : DIR_ZM;
            }
        }
        
        if (fuck++ > 1000)
            return;
    }
}
