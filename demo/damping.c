#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "forces.h"
#include "sdl_wrapper.h"
#include "render.h"

const double SCREEN_SIZE_Y = 500;
const double SCREEN_SIZE_X = 1000;
const vector_t SCREEN_SIZE = (vector_t) {1000, 500};
const size_t SIDES_APPROX = 20;
const size_t NUM_CIRCLES = 40;
const double SPRING_Y_POS = 500 * 0.5;
const double AMPLITUDE = 100;
const double SPEED = 10;
const double K = 25;
const double GAMMA = 10;
const double COIL_MASS = 1.0;
const double MASSIVE = 1000.0;

list_t *make_spring_body(scene_t *scene) {
    list_t *spring = list_init(NUM_CIRCLES, (free_func_t) body_free);
    list_t *massives = list_init(NUM_CIRCLES, (free_func_t) body_free);

    // create coils that will show in demo
    for (size_t i = 0; i < NUM_CIRCLES; i++) {
        body_t *coil = create_circle(SCREEN_SIZE_X / (NUM_CIRCLES + 1.0) / 2.0, COIL_MASS);
        double x = SCREEN_SIZE.x / (double) (NUM_CIRCLES - 1) * i;
        double y = AMPLITUDE * cos(2.0 * M_PI / NUM_CIRCLES * i) + SPRING_Y_POS;
        vector_t init_pos = {.x = x, .y = y};
        body_set_centroid(coil, init_pos);
        double y_vel = -SPEED * cos(2.0 * M_PI / NUM_CIRCLES * i);
        vector_t vel = {.x = 0, .y = y_vel};
        body_set_velocity(coil, vel);
        list_add(spring, coil);

        // create invisible masses
        body_t *invisible = create_circle(SCREEN_SIZE_X / (NUM_CIRCLES + 1.0) / 2.0, MASSIVE);
        double xmass = x;
        double ymass = SPRING_Y_POS;
        vector_t mass_pos = {.x = xmass, .y = ymass};
        body_set_centroid(invisible, mass_pos);
        list_add(massives, invisible);
    }

    if (list_size(spring) == 0) {
        return spring;
    }

    size_t num_coil = list_size(spring);
    for (size_t i = 0; i < num_coil; i++) {
        body_t *coil = list_get(spring, i);
        body_t *massive = list_get(massives, i);
        create_spring(scene, K, coil, massive);
        create_drag(scene, GAMMA / num_coil * i, coil);
    }
    return spring;
}

int main(int argc, char *argv[]) {
    // Creates the bodies
    scene_t *scene = scene_init();
    list_t *spring = make_spring_body(scene);
    for (int i = 0; i < list_size(spring); i++) {
        scene_add_body(scene, list_get(spring, i));
    }

    // Starts the game
    sdl_init(VEC_ZERO, SCREEN_SIZE);
    
    while (!sdl_is_done(NULL, NULL)) {
        double dt = time_since_last_tick();
        sdl_render_scene(scene);
        scene_tick(scene, dt);
    }

    free(spring);
    scene_free(scene);
}