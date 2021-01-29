#ifndef _EMATH_H
#define _EMATH_H

#include "global.h"

typedef struct {
    int x, y;
} vec2_t;

typedef struct {
    int x, y, z;
} vec3_t;

typedef struct {
    byte x, y, z;
} blockpos_t;

extern vec3_t cam_pos;
extern vec3_t cam_rot;
extern vec3_t cam_forward;
extern int cam_x_sin, cam_x_cos;
extern int cam_y_sin, cam_y_cos;

void vec_add(vec3_t *o, const vec3_t *a, const vec3_t *b);
void vec_sub(vec3_t* o, const vec3_t* a, const vec3_t* b);
int vec_dot(const vec3_t *a, const vec3_t *b);
void transform(vec3_t* o, const vec3_t* i);
void persp_div(vec2_t* scr, vec3_t* i);
void rot_y(vec3_t* o, const vec3_t* i, const int angle);
void rot_x(vec3_t* o, const vec3_t* i, const int angle);

#endif
