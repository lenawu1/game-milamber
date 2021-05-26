#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "compound_body.h"
#include "sdl_wrapper.h"
#include "body.h"
#include "level_handlers.h"


body_type_t *make_type_info(body_type_t type) {
    body_type_t *info = malloc(sizeof(*info));
    *info = type;
    return info;
}

body_type_t get_type(body_t *body) {
    return *(body_type_t *) body_get_info(body);
}

/** Collision handler to freeze a ball when it collides with a frozen body */
// TODO: HANDLERS
// void sand_hit(body_t *ball, body_t *target, vector_t axis, void *aux) {
//     if(body_get_info(target) == SAND) {
//         body_set_velocity(ball, VEC_ZERO);
//     }
// }

void teleport(body_t *ball, body_t *portal, vector_t axis, void *aux) {
    vector_t coords = body_get_centroid(body_get_info(portal));
    vector_t vel = body_get_velocity(ball);
    body_set_centroid(ball, vec_add(coords, vec_multiply(1, vel)));
}

void level_end(body_t *ball, body_t *target, vector_t axis, void *aux) {
    scene_t *scene = aux;
    body_type_t ball_info = get_type(ball);
    body_type_t target_info = get_type(target);
    assert(ball_info == BALL);

    if(target_info == HOLE) {
        // Win condition
        scene_add_level(scene);
        scene_set_state(scene, 1);
    }
    if(target_info == WATER) {
        // Lose condition
        scene_set_state(scene, -1);
    }
}