#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "sdl_wrapper.h"
#include "scene.h"
#include "body.h"
#include "list.h"
#include "polygon.h"

vector_t SCREEN_SIZE = (vector_t) {1000, 500};
const double ACCELERATION = 100;
const double PACMAN_RADIUS = 40;
const rgb_color_t PELLET_COLOR = (rgb_color_t) {0, 0.5, 0.5}; // dark green
const rgb_color_t PACMAN_COLOR = (rgb_color_t) {0.678, 0.737, 0.902}; // ZEN purple
const double PACMAN_OPENING_ANGLE = 0.5;
const size_t CIRCLE_APPRX = 20;
const double PELLET_RADIUS = 8;
const int NUM_PELLETS = 20; 
const double PELLETS_DELAY = 1.0;
const double BASE_SPEED = 100;
const double MASS = 0.1;


list_t *create_pie(size_t radius, double a) {
    list_t *shape = list_init(CIRCLE_APPRX, (free_func_t) free);
    vector_t *extern_points = malloc(sizeof(vector_t));
    for(int i = 0; i < CIRCLE_APPRX; i++){
        double angle = a * M_PI / CIRCLE_APPRX * i;
        extern_points->x = 0;
        extern_points->y = radius;
        vector_t *temp_v = malloc(sizeof(vector_t));
        *temp_v = vec_rotate(*extern_points, angle);
        list_add(shape, temp_v);
    }
    polygon_rotate(shape, (2.0 - a + (a / (CIRCLE_APPRX))) * M_PI / 2.0, VEC_ZERO);
    free(extern_points);
    return shape;
}

body_t *create_pacman() {
    vector_t *init_point = malloc(sizeof(vector_t));
    list_t *shape = create_pie(PACMAN_RADIUS, 2 - PACMAN_OPENING_ANGLE);
    *init_point = VEC_ZERO;
    list_add(shape, init_point);
    body_t *pacman = body_init(shape, MASS, PACMAN_COLOR); // No mass for this week
    body_set_centroid(pacman, vec_multiply(0.5, SCREEN_SIZE));
    body_set_rotation(pacman, M_PI / 2.0);
    return pacman;
}

body_t *create_pellet() {
    list_t *shape = create_pie(PELLET_RADIUS, 2);
    body_t *pellet = body_init(shape, MASS, PELLET_COLOR); // No mass for this week
    double x_pos = (float)rand()/(float)(RAND_MAX/1.0) * SCREEN_SIZE.x;
    double y_pos = (float)rand()/(float)(RAND_MAX/1.0) * SCREEN_SIZE.y;
    body_set_centroid(pellet, (vector_t) {x_pos, y_pos});
    return pellet;
}

void on_key(char key, key_event_type_t type, double held_time, scene_t *scene, list_t *aux) {
    body_t *pacman = scene_get_body(scene, 0);
    if (type == KEY_PRESSED) {
        switch (key) {
            case UP_ARROW:
                body_set_rotation(pacman, 0.0);
                body_set_velocity(pacman, (vector_t) {0, BASE_SPEED + held_time * ACCELERATION});
                break;
            case DOWN_ARROW:
                body_set_rotation(pacman, M_PI);
                body_set_velocity(pacman, (vector_t) {0, -BASE_SPEED -held_time * ACCELERATION});
                break;
            case RIGHT_ARROW:
                body_set_rotation(pacman, 3.0 * M_PI / 2.0);
                body_set_velocity(pacman, (vector_t) {BASE_SPEED + held_time * ACCELERATION, 0});
                break;
            case LEFT_ARROW:
                body_set_rotation(pacman, M_PI / 2.0);
                body_set_velocity(pacman, (vector_t) {-BASE_SPEED -held_time * ACCELERATION, 0});
                break;
        }
    }
    if (type == KEY_RELEASED) {
        switch (key) {
            case UP_ARROW:
                body_set_velocity(pacman, (vector_t) {0, BASE_SPEED});
                break;
            case DOWN_ARROW:
                body_set_velocity(pacman, (vector_t) {0, -BASE_SPEED});
                break;
            case RIGHT_ARROW:
                body_set_velocity(pacman, (vector_t) {BASE_SPEED, 0});
                break;
            case LEFT_ARROW:
                body_set_velocity(pacman, (vector_t) {-BASE_SPEED, 0});
                break;
        }
    }
}

bool check_collision(body_t *pacman, body_t *pellet) {
    vector_t pacman_centroid = body_get_centroid(pacman);
    vector_t pellet_centroid = body_get_centroid(pellet);
    vector_t displacement = vec_subtract(pacman_centroid, pellet_centroid);
    double dist_squared = pow(displacement.x, 2.0) + pow(displacement.y, 2.0);
    return dist_squared < pow(PACMAN_RADIUS, 2.0);
}

void wrap_screen(body_t *pacman) {
    vector_t pacman_centroid = body_get_centroid(pacman);
    if (pacman_centroid.x - PACMAN_RADIUS > SCREEN_SIZE.x) {
        body_set_centroid(pacman, (vector_t) {-PACMAN_RADIUS, pacman_centroid.y});
        return;
    }
    if (pacman_centroid.x + PACMAN_RADIUS < 0) {
        body_set_centroid(pacman, (vector_t) {SCREEN_SIZE.x + PACMAN_RADIUS, pacman_centroid.y});
        return;
    }
    if (pacman_centroid.y - PACMAN_RADIUS > SCREEN_SIZE.y) {
        body_set_centroid(pacman, (vector_t) {pacman_centroid.x, -PACMAN_RADIUS});
        return;
    }
    if (pacman_centroid.y + PACMAN_RADIUS < 0) {
        body_set_centroid(pacman, (vector_t) {pacman_centroid.x, SCREEN_SIZE.y + PACMAN_RADIUS});
        return;
    }
}

int main(int argc, char *argv[]) {
    // Creates the bodies
    scene_t *game_scene = scene_init();
    body_t *pacman = create_pacman();
    scene_add_body(game_scene, pacman);
    for (int i = 0; i < NUM_PELLETS; i++) {
        body_t *pellet = create_pellet();
        scene_add_body(game_scene, pellet);
    }

    // Starts the game
    sdl_init(VEC_ZERO, SCREEN_SIZE);
    sdl_on_key(on_key, pacman);
    double time_since_last_pellet = 0.0;
    
    while (!sdl_is_done(game_scene, NULL)) {
        double dt = time_since_last_tick();
        time_since_last_pellet += dt;
        // Create new pellets
        if (time_since_last_pellet > PELLETS_DELAY) {
            scene_add_body(game_scene, create_pellet());
            time_since_last_pellet = 0.0;
        }
        body_tick(pacman, dt);
        // Check collisions and eat pellets
        for (size_t i = 1; i < scene_bodies(game_scene); i++) {
            if (check_collision(pacman, scene_get_body(game_scene, i))) {
                scene_remove_body(game_scene, i);
            }
        }
        wrap_screen(pacman);
        sdl_render_scene(game_scene);
    }

    scene_free(game_scene);
}