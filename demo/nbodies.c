#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "forces.h"
#include "sdl_wrapper.h"
#include "render.h"

const double SCREEN_SIZE_Y = 500;
const double SCREEN_SIZE_X = 1000;
const vector_t SCREEN_SIZE = (vector_t) {1000, 500};
const size_t SIDES_APPROX = 20;
const size_t NUM_BODIES = 100;
const vector_t MAX_VELOCITY = (vector_t) {0, 0};
const double MIN_RAD = 5;
const double MAX_RAD = 20;
const double DENSITY = 1.0;
const double G = 20.0;

list_t *create_system(scene_t *scene) {
    list_t *system = list_init(NUM_BODIES, (free_func_t) body_free);
    for (size_t i = 0; i < NUM_BODIES; i++) {
        double radius = (double)rand()/(double)(RAND_MAX/(MAX_RAD)) + MIN_RAD;
        double mass = DENSITY * M_PI * pow(radius, 2);
        body_t *planet = create_nstar(4, radius, mass);
        body_set_centroid(planet, random_vector(VEC_ZERO, SCREEN_SIZE));
        body_set_velocity(planet, random_vector(vec_negate(MAX_VELOCITY), MAX_VELOCITY));
        list_add(system, planet);
    }
    for (size_t i = 0; i < NUM_BODIES - 1; i++) {
        body_t *planet1 = list_get(system, i);
        for (size_t j = i; j < NUM_BODIES - 1; j++) {
            body_t *planet2 = list_get(system, (size_t) (j + 1));
            create_newtonian_gravity(scene, G, planet1, planet2);
        }
    }
    return system;
}

int main(int argc, char *argv[]) {
    scene_t *scene = scene_init();
    list_t *system = create_system(scene);
    for(size_t i = 0; i < list_size(system); i++) {
        scene_add_body(scene, list_get(system, i));
    }
    //Start the sim
    sdl_init(VEC_ZERO, SCREEN_SIZE);
    
     while (!sdl_is_done(NULL, NULL)) {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    free(system);
    free(scene);
}