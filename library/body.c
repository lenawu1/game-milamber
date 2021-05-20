#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "body.h"
#include "color.h"
#include "list.h"
#include "vector.h"
#include "polygon.h"

typedef struct body {
    list_t *shape;
    double mass;
    rgb_color_t color;
    vector_t centroid;
    vector_t velocity;
    double orientation;

    vector_t force;
    vector_t impulse;
    vector_t last_impulse;

    void *info;
    list_t *collisions;
    bool collided;
    free_func_t info_freer;
    bool remove;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
    return body_init_with_info(shape, mass, color, NULL, NULL);
}

body_t *body_init_with_info(
    list_t *shape,
    double mass,
    rgb_color_t color,
    void *info,
    free_func_t info_freer
) {
    assert(mass >= 0);
    body_t *object = malloc(sizeof(body_t));
    assert(object != NULL);
    object->shape = shape;
    object->mass = mass;
    object->color = color;
    object->info = info;
    object->info_freer = info_freer;
    object->collisions = list_init(5, NULL);
    object->collided = false;

    // Initialize other values
    object->centroid = (vector_t) {0, 0};
    object->velocity = (vector_t) {0, 0};
    object->orientation = (double) 0.0;
    object->remove = false;

    object->force = VEC_ZERO;
    object->impulse = VEC_ZERO;
    object->last_impulse = VEC_ZERO;

    return object;
}

void body_free(body_t *body) {
    if (body == NULL){
        return;
    }
    list_free(body->shape);
    free(body->info);
    free(body->collisions);
    free(body);
}

list_t *body_get_shape(body_t *body) {
    // Returns newly allocated list by copying elements
    list_t *shape_cpy = list_init(10, NULL);
    for(size_t i = 0; i < list_size(body->shape); i++) {
        vector_t *temp = malloc(sizeof(vector_t));
        *temp = *(vector_t*) list_get(body->shape, i);
        list_add(shape_cpy, temp);
    }
    return shape_cpy;
}

bool body_collided(body_t *body) {
    return body->collided;
}

void body_set_collided(body_t *body, bool val) {
    body->collided = val;
}

list_t *body_get_collisions(body_t *body) {
    list_t *collisions_cpy = list_init(10, (free_func_t) body_free);
    for(size_t i = 0; i < list_size(body->collisions); i++) {
        body_t *temp = malloc(sizeof(body_t));
        *temp = *(body_t*) list_get(body->collisions, i);
        list_add(collisions_cpy, temp);
    }
    return collisions_cpy;
}

vector_t body_get_centroid(body_t *body) {
    return body->centroid;
}

vector_t body_get_velocity(body_t *body) {
    return body->velocity;
}

vector_t body_get_impulse(body_t *body) {
    return body->impulse;
}

vector_t body_get_last_impulse(body_t *body) {
    return body->last_impulse;
}

void update_last_impulse(body_t *body) {
    body->last_impulse = body->impulse;
}


double body_get_mass(body_t *body) {
    return body->mass;
}

rgb_color_t body_get_color(body_t *body) {
    return body->color;
}

void *body_get_info(body_t *body) {
    return body->info;
}

void body_translate(body_t *body, vector_t v) {
    polygon_translate(body->shape, v);
    body->centroid = vec_add(body->centroid, v);
}

void body_set_centroid(body_t *body, vector_t x) {
    vector_t cur_centroid = body->centroid;
    vector_t displacement = vec_subtract(x, cur_centroid);
    body_translate(body, displacement);
}

void body_set_velocity(body_t *body, vector_t v) {
    body->velocity = v;
}

void body_rotate_external(body_t *body, double angle, vector_t pivot) { 
    // Rotation is absolute not relative
    // Rotate body upright then rotate to new orientation?
    double rot_angle = angle - body->orientation;
    polygon_rotate(body->shape, rot_angle, pivot);
    body->orientation = angle;
    body->centroid = vec_rotate_external(body->centroid, rot_angle, pivot);
}

void body_set_rotation(body_t *body, double angle) { // Absolute angle about centroid
    body_rotate_external(body, angle, body->centroid);
}


void body_set_collisions(body_t *body, list_t *col_list) {
    body->collisions = col_list;
}

void body_set_color(body_t *body, rgb_color_t color) {
    body->color = color;
}

double body_get_rotation(body_t *body) {
    return body->orientation;
}

void body_add_force(body_t *body, vector_t force) { 
    body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
    body->impulse = vec_add(body->impulse, impulse);
}

void body_tick(body_t *body, double dt) {
    vector_t acceleration = vec_multiply((1.0 / body->mass), body->force);

    vector_t dv = vec_multiply((1.0 / body->mass), body->impulse);
    vector_t old_v = body->velocity;
    vector_t new_v = vec_add(dv, vec_add(body->velocity, vec_multiply(dt, acceleration)));
    
    vector_t avg_v = vec_multiply((0.5), vec_add(old_v, new_v));
    body_set_velocity(body, new_v);

    vector_t dx = vec_multiply(dt, avg_v);
    body_translate(body, dx);

    body->last_impulse = body->impulse;
    body->force = VEC_ZERO;
    body->impulse = VEC_ZERO;
}

void body_remove(body_t *body) {
    body->remove = true;
}

bool body_is_removed(body_t *body) {
    return body->remove;
}