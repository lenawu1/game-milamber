#ifndef __LEVEL_HANDLERS_H__
#define __LEVEL_HANDLERS_H__

#include <stdio.h>
#include <stdlib.h>
#include "compound_body.h"
#include "sdl_wrapper.h"


typedef enum {
    BALL,
    HOLE,
    GRAVITY,
    SAND,
    GRASS,
    WATER,
    BOOST,
    PORTAL
} body_type_t;

body_type_t *make_type_info(body_type_t type);

body_type_t get_type(body_t *body);

// void sand_hit(body_t *ball, body_t *target, vector_t axis, void *aux);

void teleport(body_t *ball, body_t *portal, vector_t axis, void *aux);

void level_end(body_t *ball, body_t *hole, vector_t axis, void *aux);

#endif // ifndef __LEVEL_HANDLERS_H__