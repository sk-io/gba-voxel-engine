#include "emath.h"

#include "lut.h"
#include "global.h"

vec3_t cam_pos = {TO_FP(1) / 2, TO_FP(1) / 2, TO_FP(1) / 2};
vec3_t cam_rot = {0, 0, 0};
vec3_t cam_forward;
int cam_x_sin, cam_x_cos;
int cam_y_sin, cam_y_cos;

IWRAM_CODE inline void vec_add(vec3_t *o, const vec3_t *a, const vec3_t *b) {
    o->x = a->x + b->x;
    o->y = a->y + b->y;
    o->z = a->z + b->z;
}

IWRAM_CODE inline void vec_sub(vec3_t *o, const vec3_t *a, const vec3_t *b) {
    o->x = a->x - b->x;
    o->y = a->y - b->y;
    o->z = a->z - b->z;
}

IWRAM_CODE inline int vec_dot(const vec3_t *a, const vec3_t *b) {
    return
        (a->x * b->x >> FP_SHIFT) + 
        (a->y * b->y >> FP_SHIFT) + 
        (a->z * b->z >> FP_SHIFT);
}

IWRAM_CODE inline void transform(vec3_t* o, const vec3_t *i) {
    vec3_t tmp;
    vec_sub(&tmp, i, &cam_pos);
    
    vec3_t tmp2;
    rot_y(&tmp2, &tmp, -cam_rot.y);

    rot_x(o, &tmp2, -cam_rot.x);
}

IWRAM_CODE inline void persp_div(vec2_t* scr, vec3_t *i) {
    i->x = fastdiv(i->x, i->z);
    i->y = fastdiv(i->y, i->z);

    scr->x = (i->x + TO_FP(1)) * 120;
    scr->y = (i->y + TO_FP(1)) * 80;
}

// cpuopt: merge and inline these?

IWRAM_CODE inline void rot_y(vec3_t* o, const vec3_t* i, const int angle) {
    int s = fastsin(angle), c = fastcos(angle);

    o->x = (i->z * s >> 12) + (i->x * c >> 12);
    o->y = i->y;
    o->z = (i->z * c >> 12) - (i->x * s >> 12);
}

IWRAM_CODE inline void rot_x(vec3_t* o, const vec3_t* i, const int angle) {
    int s = fastsin(angle), c = fastcos(angle);

    o->x = i->x;
    o->y = (i->y * c >> 12) - (i->z * s >> 12);
    o->z = (i->y * s >> 12) + (i->z * c >> 12);
}
