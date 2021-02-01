#include "player.h"

#include <gba_input.h>

#include "main.h"
#include "emath.h"
#include "blocks.h"
#include "raycast.h"

const int cam_y_offset = TO_FP(1) + (TO_FP(1) >> 1);

vec3_t player_pos = {TO_FP(4), TO_FP(4), TO_FP(4)};
vec3_t player_vel = {0, 0, 0};

static void calc_cam_forward() {
    const vec3_t vz = {0, 0, TO_FP(1)};

    vec3_t v1;
    rot_x(&v1, &vz, cam_rot.x);

    //vec3_t v2;
    rot_y(&cam_forward, &v1, cam_rot.y);
}

inline static void increment_pos(blockpos_t* pos, const int dir) {
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

void player_update() {
    const int move_speed = 4096;
    const int turn_speed = 80;
    vec3_t next_pos;
    vec3_t move = {0, 0, 0};

    if (!(input & KEY_R)) { // walking

        if (input & KEY_UP) {
            move.z += move_speed;
        }
        if (input & KEY_DOWN) {
            move.z -= move_speed;
        }
        if (input & KEY_LEFT) {
            move.x -= move_speed;
        }
        if (input & KEY_RIGHT) {
            move.x += move_speed;
        }
        if (input & KEY_A) {
            //move.y -= move_speed;
            player_vel.y = -600;
        }
        if (input & KEY_SELECT) {
            //move.y -= move_speed;
        }
        if (input & KEY_START) {
            //move.y += move_speed;
        }
    } else { // looking
        if (input & KEY_LEFT) {
            cam_rot.y -= turn_speed;
        }
        if (input & KEY_RIGHT) {
            cam_rot.y += turn_speed;
        }
        if (input & KEY_UP) {
            cam_rot.x += turn_speed;
        }
        if (input & KEY_DOWN) {
            cam_rot.x -= turn_speed;
        }

        if (cam_rot.x < -1024)
            cam_rot.x = -1024;
        if (cam_rot.x > 1023)
            cam_rot.x = 1023;

        calc_cam_forward();
    }

    {
        vec3_t rot_move;
        rot_y(&rot_move, &move, cam_rot.y);
        rot_move.x >>= 3;
        //move2.y >>= 3;
        rot_move.z >>= 3;
        player_vel.x = rot_move.x;
        player_vel.z = rot_move.z;
    }

    player_vel.y += 60;
    vec_add(&next_pos, &player_pos, &player_vel);

    vec3_t next_block = {TO_INT(player_pos.x), TO_INT(next_pos.y), TO_INT(player_pos.z)};

    if (blockmap[BLOCK_INDEX(next_block)] == 0) {
        // tmp
    } else {
        next_pos.y = player_pos.y;
        player_vel.y = 0;
    }
    
    player_pos = next_pos;

    cam_pos.x = player_pos.x;
    cam_pos.y = player_pos.y - cam_y_offset;
    cam_pos.z = player_pos.z;

    if (input & input_delta & KEY_B) {
        raycast();
        if (raycast_result.found) {
            blockpos_t pos = raycast_result.pos;
            increment_pos(&pos, raycast_result.face);
            blockmap[BLOCK_INDEX(pos)] = 2;
        }
    }
}
