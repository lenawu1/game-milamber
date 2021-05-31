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
const double GRASS_ELAS = 0.7;

const double LV1_HOLE_X = 1975;
const double LV1_HOLE_Y= 13;

list_t *generate_grass_shape(){
    list_t *vertices = list_init(3, free);

    list_add(vertices, vec_init_ptr(0, 0));
    list_add(vertices, vec_init_ptr(0, 500));
    list_add(vertices, vec_init_ptr(2000, 0));
    return vertices;
}

body_t *generate_grass(scene_t *scene, body_t *ball, list_t *shape) {
    body_t *grass = body_init_with_info(shape, INFINITY, GRASS_COLOR, make_type_info(GRASS), free);
    create_physics_collision(scene, GRASS_ELAS, ball, grass);
    return grass;
}

body_t *generate_water(scene_t *scene, body_t *ball, list_t *shape) {
    body_t *water = body_init_with_info(shape, INFINITY, WATER_COLOR, make_type_info(WATER), free);
    create_collision(scene, ball, water, level_end, scene, NULL);
    return water;
}

body_t *generate_sand(scene_t *scene, body_t *ball, list_t *shape) {
    body_t *sand = body_init_with_info(shape, INFINITY, SAND_COLOR, make_type_info(SAND), free);
    create_collision(scene, ball, sand, sanded, NULL, NULL);
    return sand;
}

body_t *generate_boost(scene_t *scene, body_t *ball, list_t *shape) {
    body_t *boost = body_init_with_info(shape, INFINITY, rgb_color_pastel(), make_type_info(BOOST), free);
    create_collision(scene, ball, boost, power_up, NULL, NULL);
    return boost;
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
    // body_t *gravity_source = get_gravity_body(scene);
    // scene_add_body(scene, gravity_source);
    // create_newtonian_gravity(scene, G, ball, gravity_source);
    set_frame(scene, ball, scene_get_bound(scene));

    scene_add_body(scene, generate_grass(scene, ball, generate_grass_shape()));
    list_t *hole_elements = create_golf_hole(HOLE_RADIUS, rgb_color_gray(), INFINITY);
    body_t *hole_bound = list_get(hole_elements, 0);
    body_set_centroid(hole_bound, vec_init(LV1_HOLE_X, LV1_HOLE_Y));
    create_collision(scene, ball, hole_bound, level_end, scene, NULL);
    for (size_t j = 0; j < list_size(hole_elements); j++) {
        scene_add_body(scene, list_get(hole_elements, j));
    }

    body_t *powerup = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
    body_set_centroid(powerup, vec_init(1500, 500));
    create_collision(scene, ball, powerup, power_up, scene, NULL);
    scene_add_body(scene, powerup);
    // scene_set_level(scene, 1);
}

void generate_level2(scene_t *scene, body_t *ball) {
    body_set_centroid(ball, vec_init(50, 1000));

    // body_t *gravity_source = get_gravity_body(scene);
    // scene_add_body(scene, gravity_source);
    // create_newtonian_gravity(scene, G, ball, gravity_source);
    set_frame(scene, ball, scene_get_bound(scene));

    list_t *water_s = create_rectangle_shape(1000, 280);
    body_t *water_o = generate_water(scene, ball, water_s);
    body_set_centroid(water_o, vec_init(400, 0));
    scene_add_body(scene, water_o);
    
    list_t *grass1_s = create_circle_shape(250);
    body_t *grass1_o = generate_grass(scene, ball, grass1_s);
    body_set_centroid(grass1_o, vec_init(70, 650));
    scene_add_body(scene, grass1_o);

    list_t *grass2_s = create_rectangle_shape(300, 700);
    body_t *grass2_o = generate_grass(scene, ball, grass2_s);
    body_set_centroid(grass2_o, vec_init(0, 0));
    scene_add_body(scene, grass2_o);

    list_t *grass3_s = list_init(3, free);
    list_add(grass3_s, vec_init_ptr(300, 700));
    list_add(grass3_s, vec_init_ptr(300, 0));
    list_add(grass3_s, vec_init_ptr(800, 0));
    body_t *grass3_o = generate_grass(scene, ball, grass3_s);
    scene_add_body(scene, grass3_o);

    list_t *grass4_s = list_init(3, free);
    list_add(grass4_s, vec_init_ptr(1000, 0));
    list_add(grass4_s, vec_init_ptr(1500, 400));
    list_add(grass4_s, vec_init_ptr(1500, 0));
    body_t *grass4_o = generate_grass(scene, ball, grass4_s);
    scene_add_body(scene, grass4_o);

    list_t *grass5_s = create_rectangle_shape(500, 400);
    body_t *grass5_o = generate_grass(scene, ball, grass5_s);
    body_set_centroid(grass5_o, vec_init(1500, 0));
    scene_add_body(scene, grass5_o);

    list_t *hole_elements = create_golf_hole(HOLE_RADIUS, rgb_color_gray(), INFINITY);
    body_t *hole_bound = list_get(hole_elements, 0);
    body_set_centroid(hole_bound, vec_init(1650, 400));

    create_collision(scene, ball, hole_bound, level_end, scene, NULL);
    for (size_t j = 0; j < list_size(hole_elements); j++) {
        scene_add_body(scene, list_get(hole_elements, j));
    }

    body_t *powerup1 = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
    body_set_centroid(powerup1,vec_init(500, 750));
    create_collision(scene, ball, powerup1, power_up, scene, NULL);
    scene_add_body(scene, powerup1);

    body_t *powerup2 = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
    body_set_centroid(powerup2,vec_init(1500, 1000));
    create_collision(scene, ball, powerup2, power_up, scene, NULL);
    scene_add_body(scene, powerup2);
}

void generate_level3(scene_t *scene, body_t *ball) {
    body_set_centroid(ball, vec_init(50, 1000));

    // body_t *gravity_source = get_gravity_body(scene);
    // scene_add_body(scene, gravity_source);
    // create_newtonian_gravity(scene, G, ball, gravity_source);
    set_frame(scene, ball, scene_get_bound(scene));

    list_t *sand_s = create_rectangle_shape(1000, 280);
    body_t *water_o = generate_sand(scene, ball, sand_s);
    body_set_centroid(water_o, vec_init(400, 0));
    scene_add_body(scene, water_o);
    
    list_t *grass1_s = create_circle_shape(300);
    body_t *grass1_o = generate_grass(scene, ball, grass1_s);
    body_set_centroid(grass1_o, vec_init(20, 590));
    scene_add_body(scene, grass1_o);

    list_t *grass2_s = create_rectangle_shape(300, 700);
    body_t *grass2_o = generate_grass(scene, ball, grass2_s);
    body_set_centroid(grass2_o, vec_init(0, 0));
    scene_add_body(scene, grass2_o);

    list_t *grass3_s = list_init(3, free);
    list_add(grass3_s, vec_init_ptr(300, 700));
    list_add(grass3_s, vec_init_ptr(300, 0));
    list_add(grass3_s, vec_init_ptr(800, 0));
    body_t *grass3_o = generate_grass(scene, ball, grass3_s);
    scene_add_body(scene, grass3_o);

    list_t *grass4_s = list_init(3, free);
    list_add(grass4_s, vec_init_ptr(1000, 0));
    list_add(grass4_s, vec_init_ptr(1500, 400));
    list_add(grass4_s, vec_init_ptr(1500, 0));
    body_t *grass4_o = generate_grass(scene, ball, grass4_s);
    scene_add_body(scene, grass4_o);

    list_t *grass5_s = create_rectangle_shape(500, 400);
    body_t *grass5_o = generate_grass(scene, ball, grass5_s);
    body_set_centroid(grass5_o, vec_init(1500, 0));
    scene_add_body(scene, grass5_o);

    list_t *hole_elements = create_golf_hole(HOLE_RADIUS, rgb_color_gray(), INFINITY);
    body_t *hole_bound = list_get(hole_elements, 0);
    body_set_centroid(hole_bound, vec_init(1650, 400));

    create_collision(scene, ball, hole_bound, level_end, scene, NULL);
    for (size_t j = 0; j < list_size(hole_elements); j++) {
        scene_add_body(scene, list_get(hole_elements, j));
    }
    
    body_t *powerup1 = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
    body_set_centroid(powerup1,vec_init(300, 1000));
    create_collision(scene, ball, powerup1, power_up, scene, NULL);
    scene_add_body(scene, powerup1);

    body_t *powerup2 = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
    body_set_centroid(powerup2,vec_init(500, 700));
    create_collision(scene, ball, powerup2, power_up, scene, NULL);
    scene_add_body(scene, powerup2);

    body_t *powerup3 = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
    body_set_centroid(powerup3, vec_init(850, 1250));
    create_collision(scene, ball, powerup3, power_up, scene, NULL);
    scene_add_body(scene, powerup3);
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