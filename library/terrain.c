#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "render.h"
#include "compound_body.h"
#include "collision.h"
#include "forces.h"
#include "color.h"
#include "level_handlers.h"
#include "elements.h"
#include "scene.h"

#define G 6.67E-11 // N m^2 / kg^2
#define M 6E24 // kg
#define ga 9.8 // m / s^2
#define R (sqrt(G * M / ga)) // m

const rgb_color_t WALL_COLOR = {.r = 0.0, .g = 0.0, .b = 0.0};

const int SAND_LENGTH = 5;
const int SAND_HEIGHT = 20;
const rgb_color_t SAND_COLOR = {.r = .761, .g = .698, .b = .502};

const double BUFFER = 1000;
const double WATER_LENGTH = 10;
const double WATER_HEIGHT = 50;
const double HOLE_RADIUS = 30;
const rgb_color_t WATER_COLOR = {.r = .196, .g = 0.666, .b = 0.8117};
const rgb_color_t GRASS_COLOR = {.r = .388, .g = .788, .b = 0.0};

const double SAND_ELAS = 0.0;
const double GRASS_ELAS = 0.5;

const double LV1_HOLE_X = 1975;
const double LV1_HOLE_Y= 13;

body_t *generate_sand(scene_t *scene, body_t *ball, list_t *shape) {
    body_t *sand = body_init_with_info(shape, INFINITY, SAND_COLOR, make_type_info(SAND), free);
    create_physics_collision(scene, SAND_ELAS, ball, sand);
    return sand;
    // list_t *sand = list_init(5, (free_func_t) body_free);
    // for (size_t i = 0; i < list_size(shape); i++) {
    //     body_t *one_sand = create_circle(SAND_LENGTH, INFINITY); // FIXME: Create ball not defined
    //     body_set_color(one_sand, SAND_COLOR);

    //     vector_t *sand_pos = list_get(shape, i);
    //     body_set_centroid(one_sand, *sand_pos);

    //     list_add(sand, one_sand);
    //     body_set_info(one_sand, make_type_info(SAND));
    //     create_physics_collision(scene, 0.0, one_sand, ball);
    // }
    // return sand;
}

body_t *generate_water(scene_t *scene, body_t *ball, list_t *shape) {
    // list_t *water = list_init(5, (free_func_t) body_free);
    // for (size_t i = 0; i < list_size(shape); i++) {
    //     body_t *one_water = create_rectangle(WATER_LENGTH, WATER_HEIGHT, INFINITY);
    //     body_set_color(one_water, WATER_COLOR);

    //     vector_t *water_pos = list_get(shape, i);
    //     body_set_centroid(one_water, *water_pos);
    //     list_add(water, one_water);
    //     body_set_info(one_water, make_type_info(WATER));
    //     create_collision(scene, ball, water, level_end, scene, NULL);
    // }
    // return water;
    body_t *water = body_init_with_info(shape, INFINITY, WATER_COLOR, make_type_info(WATER), free);
    create_collision(scene, ball, water, level_end, scene, NULL);
    return water;
}

list_t *generate_grass_shape(){
    list_t *vertices = list_init(3, free);

    list_add(vertices, vec_init_ptr(0, 0));
    list_add(vertices, vec_init_ptr(0, 500));
    list_add(vertices, vec_init_ptr(2000, 0));

    /*list_add(vertices, vec_init_ptr(0,0));

    list_add(vertices, vec_init_ptr(0, 250));

    list_add(vertices, vec_init_ptr(500, 150));
    list_add(vertices, vec_init_ptr(1000, 50));

    list_add(vertices, vec_init_ptr(1400, 50));
    list_add(vertices, vec_init_ptr(1800, 50));

    list_add(vertices, vec_init_ptr(1800, 0));

    list_add(vertices, vec_init_ptr(0, 0));*/
    return vertices;
}

body_t *generate_grass(scene_t *scene, body_t *ball, list_t *shape) {
    body_t *grass = body_init_with_info(shape, INFINITY, GRASS_COLOR, make_type_info(GRASS), free);
    create_physics_collision(scene, GRASS_ELAS, ball, grass);
    return grass;
}

body_t *get_gravity_body(scene_t *scene) {
    // Will be offscreen, so shape is irrelevant
    list_t *gravity_ball = create_rectangle_shape(1, 1);
    body_t *body = body_init_with_info(
        gravity_ball,
        M,
        WALL_COLOR,
        make_type_info(GRAVITY),
        free
    );

    // Move a distnace R below the scene
    vector_t gravity_center = {.x = 500, .y = -R};
    body_set_centroid(body, gravity_center);
    return body;
}

void set_frame(scene_t *scene, body_t *ball, vector_t size) {
    body_t *left = generate_grass(scene, ball, create_rectangle_shape(BUFFER, size.y + 2 * BUFFER));
    body_set_centroid(left, vec_init(-BUFFER, 0));
    body_t *right = generate_grass(scene, ball, create_rectangle_shape(BUFFER, size.y + 2 * BUFFER));
    body_set_centroid(right, vec_init(size.x, 0));

    body_t *top = generate_grass(scene, ball, create_rectangle_shape(size.x, BUFFER));
    body_set_centroid(top, vec_init(0, size.y + BUFFER));
    body_t *bottom = generate_grass(scene, ball, create_rectangle_shape(size.x + 2 * BUFFER, BUFFER));
    body_set_centroid(bottom, vec_init(-BUFFER, -BUFFER));

    scene_add_body(scene, left);
    scene_add_body(scene, right);
    scene_add_body(scene, top);
    scene_add_body(scene, bottom);
}

void generate_level1(scene_t *scene, body_t *ball) {
    body_t *gravity_source = get_gravity_body(scene);
    scene_add_body(scene, gravity_source);
    create_newtonian_gravity(scene, G, ball, gravity_source);
    set_frame(scene, ball, scene_get_bound(scene));
    scene_add_body(scene, generate_grass(scene, ball, generate_grass_shape()));
    list_t *hole_elements = create_golf_hole(HOLE_RADIUS, rgb_color_gray(), INFINITY);
    body_t *hole_bound = list_get(hole_elements, 0);
    body_set_centroid(hole_bound, vec_init(LV1_HOLE_X, LV1_HOLE_Y));
    create_collision(scene, ball, hole_bound, level_end, scene, NULL);
    for (size_t j = 0; j < list_size(hole_elements); j++) {
        scene_add_body(scene, list_get(hole_elements, j));
    }
    scene_set_level(scene, 1);
}

// Looks pretty, can use again as another level
// void set_frame(scene_t *scene, body_t *ball, vector_t size) {
//     body_t *left = generate_grass(scene, ball, create_rectangle_shape(BUFFER, size.y + 2 * BUFFER));
//     body_set_centroid(left, vec_init(-BUFFER / 2, size.y / 2));
//     body_t *right = generate_grass(scene, ball, create_rectangle_shape(BUFFER, size.y + 2 * BUFFER));
//     body_set_centroid(right, vec_init(size.x + BUFFER / 2, size.y / 2));

//     body_t *top = generate_grass(scene, ball, create_rectangle_shape(size.x, BUFFER));
//     body_set_centroid(top, vec_init(size.x / 2, size.y + BUFFER / 2));
//     body_t *bottom = generate_grass(scene, ball, create_rectangle_shape(size.x, BUFFER));
//     body_set_centroid(bottom, vec_init(size.x / 2, -BUFFER / 2));

//     scene_add_body(scene, left);
//     scene_add_body(scene, right);
//     scene_add_body(scene, top);
//     scene_add_body(scene, bottom);
// }