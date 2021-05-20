#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>
#include "compound_body.h"

typedef struct compound_body {
    list_t *bodies;
    size_t size;
    double mass;
    vector_t centroid;
    vector_t velocity;
    double orientation;

    // vector_t last_impulse;

    bool collided;
    bool remove;
} comp_body_t;

comp_body_t *comp_body_init() {
    comp_body_t *new_comp = malloc(sizeof(comp_body_t));
    assert(new_comp != NULL);
    list_t *bodies_list = list_init(2, (free_func_t) body_free);
    new_comp->bodies = bodies_list;
    new_comp->size = 0;
    new_comp->mass = 0;
    new_comp->centroid = VEC_ZERO;
    new_comp->velocity = VEC_ZERO;
    new_comp->orientation = 0;
    new_comp->collided = false;
    new_comp->remove = false;
    return new_comp;
}

void comp_body_free(comp_body_t *comp) {
    if (comp->size == 0) {
        free(comp);
        return;
    }
    list_free(comp->bodies);
    free(comp);
}

void comp_body_add(comp_body_t *comp, body_t *body) {
    if (comp->size == 0) {
        comp->centroid = body_get_centroid(body);
        comp->velocity = body_get_velocity(body);
        comp->orientation = body_get_rotation(body);
        comp->collided = body_collided(body);
        comp->remove = body_is_removed(body);
    }
    list_add(comp->bodies, body);
    comp->mass += body_get_mass(body);
    comp->size++;
}

list_t *get_internal_bodies(comp_body_t *comp) {
    list_t *bodies_list = list_init(comp->size, (free_func_t) body_free);
    for (size_t i = 0; i < comp->size; i++) {
        list_add(bodies_list, list_get(comp->bodies, i));
    }
    return bodies_list;
}

bool comp_body_collided(comp_body_t *comp) {
    return comp->collided;
}

void comp_body_set_collided(comp_body_t *comp, bool val) {
    for (size_t i = 0; i < comp->size; i++) {
        body_set_collided(list_get(comp->bodies, i), val);
    }
    comp->collided = val;
}

vector_t comp_body_get_centroid(comp_body_t *comp) {
    return comp->centroid;
}

vector_t comp_body_get_velocity(comp_body_t *comp) {
    return comp->velocity;
}

double comp_body_get_mass(comp_body_t *comp) {
    return comp->mass;
}

void comp_body_set_centroid(comp_body_t *comp, vector_t x) {
    vector_t displacement = vec_subtract(x, comp->centroid);
    for (size_t i = 0; i < comp->size; i++) {
        body_t *body = list_get(comp->bodies, i);
        body_translate(body, displacement);
    }
    comp->centroid = x;
}

void comp_body_set_velocity(comp_body_t *comp, vector_t v) {
    for (size_t i = 0; i < comp->size; i++) {
        body_t *body = list_get(comp->bodies, i);
        body_set_velocity(body, v);
    }
    comp->velocity = v;
}

void comp_body_set_rotation(comp_body_t *comp, double angle) {
    vector_t pivot = comp->centroid;
    for (size_t i = 0; i < comp->size; i++) {
        body_t *body = list_get(comp->bodies, i);
        double body_rel_angle = body_get_rotation(body) - comp->orientation;
        body_rotate_external(body, angle + body_rel_angle, pivot);
    }
    comp->orientation = angle;
}

double comp_body_get_rotation(comp_body_t *comp) {
    return comp->orientation;
}


void comp_body_add_force(comp_body_t *comp, vector_t force) {
    for (size_t i = 0; i < comp->size; i++) {
        body_t *body = list_get(comp->bodies, i);
        body_add_force(body, force);
    }
}

void comp_body_add_impulse(comp_body_t *comp, vector_t impulse) {
    for (size_t i = 0; i < comp->size; i++) {
        body_t *body = list_get(comp->bodies, i);
        body_add_impulse(body, impulse);
    }
}

void comp_body_tick(comp_body_t *comp, double dt) {
    for (size_t i = 0; i < comp->size; i++) {
        body_t *body = list_get(comp->bodies, i);
        body_tick(body, dt);
    }
}

void comp_body_remove(comp_body_t *comp) {
    for (size_t i = 0; i < comp->size; i++) {
        body_t *body = list_get(comp->bodies, i);
        body_remove(body);
    }
    comp->remove = true;
}

bool comp_body_is_removed(comp_body_t *comp) {
    return comp->remove;
}