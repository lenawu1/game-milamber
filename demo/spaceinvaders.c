#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "list.h"
#include "vector.h"
#include "sdl_wrapper.h"
#include "scene.h"
#include "body.h"
#include "color.h"
#include "render.h"
#include "collision.h"
#include <math.h>
#include "polygon.h"
#include "forces.h"

const int STARTER_ENEMIES = 21;
const double SCREEN_SIZE_Y = 1000;
const double SCREEN_SIZE_X = 750;
const vector_t SCREEN_SIZE = {.x = 750, .y = 1000};
const double SPEED = 60;
const double MASS = 10.0;
const double RADIUS = 30;
const double PLAYER_POS = 40;
const double FROM_EDGE = 70;

list_t *create_enemies(scene_t *scene) {
    list_t *enemies = list_init(STARTER_ENEMIES, (free_func_t) body_free);
    //creates all enemies in rectangular formation
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 7; j++) {
            body_t *enemy = create_enemy(RADIUS, MASS);
            double x = 40 + j*75;
            double y = SCREEN_SIZE_Y - 40 - i*70;
            vector_t init_pos = {.x = x, .y = y};
            body_set_centroid(enemy, init_pos);
            double x_vel = SPEED;
            vector_t vel = {.x = x_vel, .y = 0};
            body_set_velocity(enemy, vel);
            list_add(enemies, enemy);
        }
    }
    for (int i = 0; i < STARTER_ENEMIES; i++) {
        scene_add_body(scene, list_get(enemies, i));
    }
    return enemies;
}

body_t *create_player(scene_t *scene) {
    body_t *player = body_init(create_oval_shape(RADIUS, RADIUS/2), MASS, rgb_color_yellow());
    vector_t player_pos = {.x = SCREEN_SIZE.x/2.0, .y = PLAYER_POS};
    body_set_centroid(player, player_pos);
    scene_add_body(scene, player);
    return player;
}

void generate_bullet(scene_t *scene, list_t *bullets, body_t *player, bool is_enemy) {
    body_t *bullet = create_bullet(10, 20, MASS, is_enemy);
    if (is_enemy) {
        int rand_body = rand() % ((int) scene_bodies(scene) - 1);
        vector_t pos = {.x = body_get_centroid(scene_get_body(scene, rand_body)).x, 
                            .y = body_get_centroid(scene_get_body(scene, rand_body)).y};
        body_set_centroid(bullet, pos);
        vector_t vel = {.x = 0, .y = -500};
        body_set_velocity(bullet, vel);
        scene_add_body(scene, bullet);
    }
    else {
        vector_t pos = {.x = body_get_centroid(player).x, .y = body_get_centroid(player).y + 5};
        body_set_centroid(bullet, pos);
        vector_t vel = {.x = 0, .y = 500};
        body_set_velocity(bullet, vel);
        scene_add_body(scene, bullet);
    }
    list_add(bullets, bullet);
}

void move_enemies(list_t *enemies) {
    for(int i = 0; i < list_size(enemies); i++) {
        list_t *enemy_shape = body_get_shape(list_get(enemies, i));
        body_t *enemy = list_get(enemies, i);
        vector_t curr_centroid = body_get_centroid(enemy);
        if ((curr_centroid.x >= SCREEN_SIZE.x - RADIUS)) { 
            vector_t new_pos = {.x = curr_centroid.x - 3, .y = curr_centroid.y - 235};
            body_set_velocity(enemy, vec_negate(body_get_velocity(enemy)));
            body_set_centroid(enemy, new_pos);
        }
        else if ((curr_centroid.x <= RADIUS)) { 
            vector_t new_pos = {.x = curr_centroid.x + 3, .y = curr_centroid.y - 235};
            body_set_velocity(enemy, vec_negate(body_get_velocity(enemy)));
            body_set_centroid(enemy, new_pos);
        }
        list_free(enemy_shape);
    }
}

void handler(char key, key_event_type_t type, double held_time, scene_t *scene, list_t *player_bullets) {
    body_t *player = scene_get_body(scene, 0);

    if (type == KEY_PRESSED) {
        held_time += 1.5;
        if (key == LEFT_ARROW) {
            if(body_get_centroid(player).x <= RADIUS * 2) {
                held_time = 0;
                vector_t velocity = {0, 0};
                body_set_velocity(player, velocity);
            }
            vector_t left_v = {.x = -150.0, .y = 0};
            body_set_velocity(player, vec_multiply(held_time, left_v));
        }
        else if (key == RIGHT_ARROW) {
            if(body_get_centroid(player).x >= SCREEN_SIZE.x - RADIUS * 2) {
                held_time = 0;
                vector_t velocity = {0, 0};
                body_set_velocity(player, velocity);
            }
            vector_t right_v = {.x = 150.0, .y = 0};
            body_set_velocity(player, vec_multiply(held_time, right_v));
        }
        else if (key == SPACE) {
            generate_bullet(scene, player_bullets, player, false);
        }
    }
    else {
        held_time = 0;
        vector_t velocity = {0, 0};
        body_set_velocity(player, velocity);
    } 
}

bool player_shot(scene_t *scene, list_t *bullets) {
    body_t *player = scene_get_body(scene, 0);
    for (size_t i = 0; i < list_size(bullets); i++) {
        body_t *bullet = list_get(bullets, i);
        if (find_collision(body_get_shape(bullet), body_get_shape(player)).collided) {
            body_remove(player);
            return true;
        }
    }
    return false;
}

void enemy_shot(scene_t *scene, list_t *bullets, list_t *enemies) {
    for (size_t i = 0; i < list_size(bullets); i++) {
        body_t *bullet = list_get(bullets, i);
        for (int i = 0; i < list_size(enemies); i++) {
            if(find_collision(body_get_shape(bullet), body_get_shape(list_get(enemies, i))).collided) {
                body_remove(list_get(enemies, i));
                list_remove(enemies, i);
            }
        }  
    } 
}


int main(int argc, char *argv[]) {
    scene_t *scene = scene_init();

    body_t *player = create_player(scene);
    list_t *enemies = create_enemies(scene);
    list_t *enemy_bullets = list_init(10, (free_func_t) free);
    list_t *player_bullets = list_init(10, (free_func_t) free);

    sdl_init(VEC_ZERO, SCREEN_SIZE);
    sdl_on_key(handler,scene);

    double clock = 0.0;

    while (!sdl_is_done(scene, player_bullets)) {
        double dt = time_since_last_tick();
        clock += dt;

        if (clock >= 1.0){ //enemy generate bullet
            generate_bullet(scene, enemy_bullets, player, true);
            clock = 0.0;
        }

        enemy_shot(scene, player_bullets, enemies);
        if (player_shot(scene, enemy_bullets) || list_size(enemies) == 0) {
            break;
        }

        move_enemies(enemies);
        sdl_render_scene(scene);
        scene_tick(scene, dt);
    }

    scene_free(scene);
}