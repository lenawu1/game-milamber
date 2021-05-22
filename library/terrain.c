#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "render.h"
#include "compound_body.h"
#include "collision.h"
#include "forces.h"

const int SAND_LENGTH = 5;
const int SAND_HEIGHT = 20;
const rgb_color_t SAND_COLOR = {.r = .761, .g = .698, .b = .502};


bool level_end(comp_body_t *ball, comp_body_t *hole) {
    if (list_contains(body_get_collisions(ball), hole)) {
        return true;
    }
    return false;
}

comp_body_t *generate_sand(scene_t *scene, comp_body_t *ball, list_t *shape) {
    comp_body_t *sand = comp_body_init();
    for (size_t i = 0; i < list_size(shape); i++) {
        body_t *one_sand = create_ball(SAND_LENGTH, SAND_HEIGHT, INFINITY);
        body_set_color(one_sand, SAND_COLOR);

        vector_t *sand_pos = list_get(shape, i);
        body_set_centroid(one_sand, *sand_pos);

        comp_body_add(sand, one_sand);
        create_physics_collision(scene, 0.0, one_sand, ball);
    }
    return sand;
}
