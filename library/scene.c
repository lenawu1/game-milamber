#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "forces.h"
#include "compound_body.h"
#include "scene.h"
#include "math.h"
#include "list.h"
#include "collision.h"

const size_t INIT_CAPACITY = 100;

typedef struct scene {
    list_t *bodies;
    list_t *force_bundles;
    size_t points;
} scene_t;

typedef struct force {
    force_creator_t forcer;
    void *aux;
    list_t *bodies;
    free_func_t freer;
} force_bundle_t;

force_bundle_t *force_bundle_init(force_creator_t forcer, void *aux, list_t *bodies, free_func_t freer) {
    force_bundle_t *new_force_bundle = malloc(sizeof(force_bundle_t));
    new_force_bundle->forcer = forcer;
    new_force_bundle->aux = aux;
    new_force_bundle->bodies = bodies;
    new_force_bundle->freer = freer;
    return new_force_bundle;
}

void force_bundle_free(force_bundle_t *force_bundle) {
    force_bundle->freer(force_bundle->aux);
    free(force_bundle->bodies);
    free(force_bundle);
}

list_t *get_force_bundle_bodies(force_bundle_t *force_bundle) {
    return force_bundle->bodies;
}

force_creator_t get_force_bundle_forcer(force_bundle_t *force_bundle) {
    return force_bundle->forcer;
}

scene_t *scene_init(void) {
    scene_t *scene = malloc(sizeof(scene_t));
    assert(scene != NULL);
    scene->bodies = list_init(INIT_CAPACITY, (free_func_t) comp_body_free);
    scene->force_bundles = list_init(INIT_CAPACITY, (free_func_t) force_bundle_free);
    scene->points = 0;
    return scene;
}

void scene_free(scene_t *scene) {
    list_free(scene->bodies);
    list_free(scene->force_bundles);
    free(scene);
}

size_t scene_bodies(scene_t *scene) {
    return list_size(scene->bodies);
}

body_t *scene_get_body(scene_t *scene, size_t index) {
    assert(index < scene_bodies(scene));
    return list_get(scene->bodies, index);
}

list_t *scene_get_force_bundles(scene_t *scene) {
    return scene->force_bundles;
}

size_t scene_get_points(scene_t *scene) {
    return scene->points;
}

void scene_add_body(scene_t *scene, body_t *body) {
    list_add(scene->bodies, body);
}

size_t scene_add_point(scene_t *scene) {
    scene->points++;
    return scene->points;
}

void scene_remove_body(scene_t *scene, size_t index) {
    assert(index <= list_size(scene->bodies));
    comp_body_t *removed = list_remove(scene->bodies, index);
    comp_body_free(removed);
}

void scene_add_force_creator(
    scene_t *scene,
    force_creator_t forcer,
    void *aux,
    free_func_t freer
) {
    scene_add_bodies_force_creator(scene, forcer, aux, NULL, freer);
}

void scene_add_bodies_force_creator(
    scene_t *scene,
    force_creator_t forcer,
    void *aux,
    list_t *bodies,
    free_func_t freer
) {
    force_bundle_t *new_force_bundle = force_bundle_init(forcer, aux, bodies, freer);
    list_add(scene->force_bundles, new_force_bundle);
}

void scene_tick(scene_t *scene, double dt) { // Adding the force creators
    for (size_t i = 0; i < list_size(scene->force_bundles); i++) {
        force_bundle_t *curr_force_bundle = list_get(scene->force_bundles, i);
        force_creator_t curr_force_creator = curr_force_bundle->forcer;
        
        for (size_t j = 0; j < list_size(curr_force_bundle->bodies); j++) {
            // Single Bodies
            if(curr_force_creator == (force_creator_t) drag_force_creator) {
                list_t *body_single = list_init(2, free);
                list_add(body_single, list_get(curr_force_bundle->bodies, j));
                curr_force_creator(curr_force_bundle->aux, body_single);
            }
            // Multiple bodies
            else{
                list_t *body_pair = list_init(2, free);
                list_add(body_pair, list_get(curr_force_bundle->bodies, j));
                list_add(body_pair, list_get(curr_force_bundle->bodies, (j+1) % list_size(curr_force_bundle->bodies)));
                curr_force_creator(curr_force_bundle->aux, body_pair);
            }

        }
    }

    for (size_t i = 0; i < scene_bodies(scene); i++) { // Remove bodies & forces if necessary
        comp_body_t *curr_body = comp_body_init();
        comp_body_add(curr_body, scene_get_body(scene, i));
        if (comp_body_is_removed(curr_body)) {
            for (size_t j = 0; j < list_size(scene->force_bundles); j++) {
                force_bundle_t *force_bundle = list_get(scene->force_bundles, j);
                list_t *bodies = force_bundle->bodies;
                if (list_contains(bodies, curr_body)) {
                    force_bundle_free(list_remove(scene->force_bundles, j)); 
                    j--;
                }
            }
            list_remove(scene->bodies, i);
            i--;
            continue;
        }
        comp_body_tick(curr_body, dt);
        // body_set_collided(curr_body, false);
    }
}