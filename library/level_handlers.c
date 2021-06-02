#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sdl_wrapper.h"
#include "compound_body.h"
#include "body.h"
#include "level_handlers.h"
#include "terrain.h"
#include "elements.h"
#include "polygon.h"

const int LEVELS = 5;

char level_data[5][50] = {
                            "resources/level1.txt",
                            "resources/level2.txt",
                            "resources/level3.txt",
                            "resources/level4.txt",
                            "resources/level5.txt",
                            };

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

teleport_aux_t *make_teleport_aux(body_t *out, vector_t dir) {
    teleport_aux_t *aux = malloc(sizeof(teleport_aux_t));
    aux->direction = dir;
    aux->out = out;
    return aux;
}

void teleport(body_t *ball, body_t *portal, vector_t axis, void *aux) {
    body_t *out = ((teleport_aux_t*) aux)->out;
    vector_t dir = ((teleport_aux_t*) aux)->direction;
    body_set_centroid(ball, polygon_centroid(body_get_shape(out)));
    vector_t cur_v = body_get_velocity(ball);
    vector_t new_v = vec_multiply(-vec_norm(cur_v), dir);
    body_set_velocity(ball, new_v);
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

void power_up(body_t *ball, body_t *target, vector_t axis, void *aux) {
    body_type_t ball_info = get_type(ball);
    assert(ball_info == BALL);
    body_set_velocity(ball, vec_multiply(2.0, body_get_velocity(ball)));
    body_remove(target);
}

body_t *build_level(scene_t *scene) {
    size_t level = scene_get_level(scene);
    body_t *player;
    // Array of all the resource json files for each level
    // Access the specific file
    // Build the terrain based on that file
    // If level == 1, make sure we create the ball since its not recreated each time
    if (level == 1 && scene_get_first_try(scene)) 
    {
        list_t *ball_elements = create_golf_ball(BALL_SIZE, rgb_color_init(205, 99, 75), BALL_MASS, INIT_POS1);
         player = list_get(ball_elements, 0);

        for(size_t i = 0; i < list_size(ball_elements); i++) {
            scene_add_body(scene, list_get(ball_elements, i));
        }
        generate_level(scene, player, level_data[level - 1]);
        scene_set_first_try(scene, false);
    } 
    else {
        if(level > LEVELS) { // TODO: hen we get the last level make some special screen
            printf("Invalid level. \n");
            exit(0);
        }
        player = scene_get_body(scene, 0);
        generate_level(scene, player, level_data[level - 1]);
    }
    return player;
}