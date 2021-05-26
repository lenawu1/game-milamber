#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "render.h"
#include "compound_body.h"
#include "collision.h"
#include "forces.h"
#include "color.h"

const int SAND_LENGTH = 5;
const int SAND_HEIGHT = 20;
const rgb_color_t SAND_COLOR = {.r = .761, .g = .698, .b = .502};

const double WATER_LENGTH = 10;
const double WATER_HEIGHT = 50;
const rgb_color_t WATER_COLOR = {.r = .529, .g = 0.808, .b = 0.980};


bool level_end(comp_body_t *ball, comp_body_t *hole, comp_body_t *water) {
    list_t *collisions = body_get_collisions(ball);
    if (list_contains(collisions, hole) || list_contains(collisions, water)) {
        return true;
    }
    return false;
}

comp_body_t *generate_sand(scene_t *scene, comp_body_t *ball, list_t *shape) {
    comp_body_t *sand = comp_body_init();
    for (size_t i = 0; i < list_size(shape); i++) {
        body_t *one_sand = create_ball(SAND_LENGTH, SAND_HEIGHT, INFINITY); // FIXME: Create ball not defined
        body_set_color(one_sand, SAND_COLOR);

        vector_t *sand_pos = list_get(shape, i);
        body_set_centroid(one_sand, *sand_pos);

        comp_body_add(sand, one_sand);
        create_physics_collision(scene, 0.0, one_sand, ball);
    }
    return sand;
}

comp_body_t *generate_water(scene_t *scene, comp_body_t *ball, list_t *shape) {
    comp_body_t *water = comp_body_init();
    for (size_t i = 0; i < list_size(shape); i++) {
        body_t *one_water = create_rectangle(WATER_LENGTH, WATER_HEIGHT, INFINITY);
        body_set_color(water, WATER_COLOR);

        vector_t *water_pos = list_get(shape, i);
        body_set_centroid(water, *water_pos);

        comp_body_add(water, one_water);
    }
    return water;
}
