#include <stdio.h>
#include <stdlib.h>
#include "compound_body.h"
#include "sdl_wrapper.h"
#include "body.h"

/** Collision handler to freeze a ball when it collides with a frozen body */
// FIXME: HANDLERS
void freeze(body_t *ball, body_t *target, vector_t axis, void *aux) {
    if(body_get_info(target) == SAND) {
        body_set_velocity(ball, VEC_ZERO);
    }
}