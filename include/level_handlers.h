#ifndef __LEVEL_HANDLERS_H__
#define __LEVEL_HANDLERS_H__

#include <stdio.h>
#include <stdlib.h>
#include "compound_body.h"
#include "sdl_wrapper.h"


typedef enum {
    BALL,
    SAND,
    GRASS,
    BOOST
} body_type_t;

void freeze(body_t *ball, body_t *target, vector_t axis, void *aux);


#endif // ifndef __LEVEL_HANDLERS_H__