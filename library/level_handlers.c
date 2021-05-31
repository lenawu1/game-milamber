#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "compound_body.h"
#include "sdl_wrapper.h"
#include "body.h"
#include "level_handlers.h"
#include "terrain.h"
#include "elements.h"


const double BALL_SIZE = 20;
const vector_t INIT_POS1 = {.x = 50, .y = 1000};
const double BALL_MASS = 40.0;

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
        // scene_add_level(scene);
        scene_set_state(scene, 1);
    }
    if(target_info == WATER) {
        // Lose condition
        scene_set_state(scene, -1);
    }
    body_set_velocity(ball, VEC_ZERO);
}

void sanded(body_t *ball, body_t *target, vector_t axis, void *aux) {
    body_type_t ball_info = get_type(ball);
    assert(ball_info == BALL);
    body_set_velocity(ball, VEC_ZERO);
}

void poweredup(body_t *ball, body_t *target, vector_t axis, void *aux) {
    body_type_t ball_info = get_type(ball);
    assert(ball_info == BALL);
    body_set_velocity(ball, vec_multiply(1.2, body_get_velocity(ball)));
}

body_t *build_level(scene_t *scene) {
    size_t level = scene_get_level(scene);
    printf("%zu \n", level);
    body_t *player;
    // TODO: Get the nth level file path from some array
    if (level == 1) {
        list_t *ball_elements = create_golf_ball(BALL_SIZE, rgb_color_init(205, 99, 75), BALL_MASS, INIT_POS1);
         player = list_get(ball_elements, 0);

        for(size_t i = 0; i < list_size(ball_elements); i++) {
            scene_add_body(scene, list_get(ball_elements, i));
        }
        generate_level1(scene, player);
    } else if (level == 2) {
        // list_t *ball_elements = create_golf_ball(BALL_SIZE, rgb_color_init(205, 99, 75), BALL_MASS, INIT_POS1);
        player = scene_get_body(scene, 0);
        generate_level2(scene, player);
    } else if (level == 3) {
        // list_t *ball_elements = create_golf_ball(BALL_SIZE, rgb_color_init(205, 99, 75), BALL_MASS, INIT_POS1);
        player = scene_get_body(scene, 0);
        generate_level3(scene, player);
    }
    else{
        printf("Unknown level\n");
        return NULL;
    }

    return player;
}