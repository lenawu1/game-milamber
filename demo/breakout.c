#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "list.h"
#include "vector.h"
#include "sdl_wrapper.h"
#include "scene.h"
#include "forces.h"
#include "body.h"
#include "color.h"
#include "render.h"
#include "collision.h"
#include "polygon.h"

const int STARTER_BLOCKS = 30;
const double SCREEN_SIZE_Y = 750;
const double SCREEN_SIZE_X = 750;
const vector_t SCREEN_SIZE = {.x = 750, .y = 750};
const double PLAYER_SPEED = 700;
const double MASS = 10.0;
const double RADIUS = 30;
const double BLOCK_LENGTH = 63;
const double BLOCK_HEIGHT = 25;
const double BALL_SIZE = 6;
const double PLAYER_POS = 20;
const double LARGE_MASS = INFINITY;
const double WALL_THICKNESS = 10;

body_t *generate_player(scene_t *scene) {
    body_t *player = body_init(create_rectangle_shape(BLOCK_LENGTH, BLOCK_HEIGHT), LARGE_MASS, rgb_color_pastel());

    vector_t player_pos = {.x = SCREEN_SIZE.x/2.0, .y = PLAYER_POS};
    body_set_centroid(player, player_pos);

    scene_add_body(scene, player);

    return player;
}

void remove_colliding_bodies(body_t *body1, body_t *body2, vector_t axis, void *aux) { 
    if (rgb_color_equal(body_get_color(body2), rgb_color_gray())){
        body_remove(body2);
    }
    else {
        body_set_color(body2, rgb_color_gray());
    }
}

list_t *generate_blocks(scene_t *scene, body_t *ball) {
    list_t *blocks = list_init(STARTER_BLOCKS, (free_func_t) body_free);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 10; j++) {
            body_t *block = create_rectangle(BLOCK_LENGTH, BLOCK_HEIGHT, LARGE_MASS, j);
            
            double x = 20 + 72 * j;
            double y = SCREEN_SIZE_Y - 45 - i*40;
            vector_t init_pos = {.x = x, .y = y};

            body_set_centroid(block, init_pos);
            list_add(blocks, block);

            create_physics_collision(scene, 1.0, block, ball);
            create_collision(scene, ball, block, remove_colliding_bodies, NULL, free);
        }
    }
    for (int i = 0; i < STARTER_BLOCKS; i++) {
        scene_add_body(scene, list_get(blocks, i));
    }
    return blocks;
}


body_t *generate_ball(scene_t *scene, body_t *player) {
    body_t *ball = create_ball(BALL_SIZE, BALL_SIZE, MASS);
    
    vector_t pos = {
        .x = body_get_centroid(player).x + BLOCK_LENGTH/2, 
        .y = body_get_centroid(player).y + 50
    };
    body_set_centroid(ball, pos);

    vector_t vel = {.x = 150, .y = 200};
    body_set_velocity(ball, vel);

    scene_add_body(scene, ball);

    create_physics_collision(scene, 1.0, ball, player);
    
    return ball;
}

void player_in_bounds(scene_t *scene){
    body_t *player = scene_get_body(scene, 0);

    bool left_wall = body_get_centroid(player).x <= 2 && body_get_velocity(player).x <= 0;
    bool right_wall = body_get_centroid(player).x >= SCREEN_SIZE_X - 58 && body_get_velocity(player).x >= 0;

    if (left_wall || right_wall) {
        body_set_velocity(player, VEC_ZERO);
    }
}

void generate_walls(scene_t *scene, body_t *ball) {
    body_t *right_wall = create_wall(WALL_THICKNESS, SCREEN_SIZE_Y, LARGE_MASS);
    body_t *left_wall = create_wall(WALL_THICKNESS, SCREEN_SIZE_Y, LARGE_MASS);

    vector_t right_pos = {.x = SCREEN_SIZE_X, .y = 0};
    vector_t left_pos = {.x = -1 * WALL_THICKNESS, .y = 0};

    body_set_centroid(right_wall, right_pos);
    body_set_centroid(left_wall, left_pos);

    scene_add_body(scene, right_wall);
    scene_add_body(scene, left_wall);

    create_physics_collision(scene, 1.0, right_wall, ball);
    create_physics_collision(scene, 1.0, left_wall, ball);
}

void handler(char key, key_event_type_t type, double held_time, scene_t *scene, list_t *null_list) {
    body_t *player = scene_get_body(scene, 0);
    if (type == KEY_PRESSED) {
        held_time += 1.5;
        if (key == LEFT_ARROW) {
            vector_t left_v = {.x = -1.0*PLAYER_SPEED, .y = 0};
            body_set_velocity(player, vec_multiply(held_time, left_v));
        }
        else if (key == RIGHT_ARROW) {
            vector_t right_v = {.x = PLAYER_SPEED, .y = 0};
            body_set_velocity(player, vec_multiply(held_time, right_v));
        }
    }
    else {
        held_time = 0;
        body_set_velocity(player, VEC_ZERO);
    } 
}

bool should_restart_game(scene_t *scene, body_t *ball, list_t *blocks) {
    bool ball_out_of_bounds =  (body_get_centroid(ball).y <= 0);
    bool blocks_all_gone = (list_size(blocks) == 0);
    return (ball_out_of_bounds || blocks_all_gone);
}

int main(int argc, char *argv[]) {
    sdl_init(VEC_ZERO, SCREEN_SIZE);

    scene_t *scene = scene_init();
    body_t *player = generate_player(scene);

    body_t *ball = generate_ball(scene, player);
    list_t *blocks = generate_blocks(scene, ball);
    generate_walls(scene, ball);

    sdl_on_key(handler,scene);
    
    double clock = 0.0;
    
    while (!sdl_is_done(scene, NULL)){
        double dt = time_since_last_tick();
        clock += dt;

        if (should_restart_game(scene, ball, blocks)) {
            for (size_t i = 1; i < scene_bodies(scene); i++) {
                body_remove(scene_get_body(scene, i));
            }

            ball = generate_ball(scene, player);
            blocks = generate_blocks(scene, ball);
            generate_walls(scene, ball);

            clock = 0;
        }
        player_in_bounds(scene);
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
}