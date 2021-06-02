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
#include "cJSON.h"

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

body_t *generate_spike(scene_t *scene, body_t *ball, list_t *shape) {
    body_t *spike = body_init_with_info(shape, INFINITY, rgb_color_gray(), make_type_info(WATER), free);
    create_collision(scene, ball, spike, level_end, scene, NULL);
    return spike;
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

char* read_file(char *path) {
    FILE *f = fopen(path, "r");
    
    if (f == NULL) {
        return NULL;
    }

    char *result = malloc(sizeof(char));
    char *res = malloc(100);
    res[0] = '\0'; // initialize longest word
    size_t current_length = 0;

    int successful = fread(result, 1, 1, f); // reads first character
    // fread in while as before
    while (successful == 1) {
        res[current_length] = result[0];
        res = realloc(res, current_length + 100);
        current_length++;
        successful = fread(result, 1, 1, f);
    }
    res[current_length] = '\0';
    free(result);
    fclose(f);
    return res;
}

void generate_level(scene_t *scene, body_t *ball, char* level) {
    char *data = read_file(level);
    const cJSON *bounds = NULL;
    const cJSON *objects = NULL;
    const cJSON *object = NULL;
    int status = 0;
    cJSON *monitor_json = cJSON_Parse(data);
    if (monitor_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
        goto end;
    }
    bounds = cJSON_GetObjectItemCaseSensitive(monitor_json, "bounds");
    cJSON *width = cJSON_GetObjectItemCaseSensitive(bounds, "width");
    cJSON *height = cJSON_GetObjectItemCaseSensitive(bounds, "height");
    vector_t bound_val = vec_init(width->valuedouble, height->valuedouble);

    set_frame(scene, ball, bound_val);

    objects = cJSON_GetObjectItemCaseSensitive(monitor_json, "objects");
    cJSON_ArrayForEach(object, objects)
    {
        cJSON *type_p = cJSON_GetObjectItemCaseSensitive(object, "type");
        cJSON *pos_x_p = cJSON_GetObjectItemCaseSensitive(object, "pos_x");
        cJSON *pos_y_p = cJSON_GetObjectItemCaseSensitive(object, "pos_y");
        cJSON *shape_p = NULL;
        list_t *shape = NULL;
        char* type = type_p->valuestring;
        double pos_x = pos_x_p->valuedouble;
        double pos_y = pos_y_p->valuedouble;
        
        if(strcmp(type, "GRASS") == 0 || strcmp(type, "WATER") == 0 || strcmp(type, "SAND") == 0) {
            shape_p = cJSON_GetObjectItemCaseSensitive(object, "shape");
            cJSON *vertex_p = NULL;
            shape = list_init(5, free);
            cJSON_ArrayForEach(vertex_p, shape_p)
            {
                cJSON *x_p = cJSON_GetObjectItemCaseSensitive(vertex_p, "x");
                cJSON *y_p = cJSON_GetObjectItemCaseSensitive(vertex_p, "y");
                vector_t *vertex = vec_init_ptr(x_p->valuedouble, y_p->valuedouble);
                list_add(shape, vertex);
            }
        }
        // BALL, GRASS, CIRCLE_GRASS, POWER, SPIKE, WATER, SAND, HOLE
        if(strcmp(type, "BALL") == 0) {
            body_set_centroid(ball, vec_init(pos_x, pos_y));
        }
        else if(strcmp(type, "HOLE") == 0) {
            list_t *hole_elements = create_golf_hole(HOLE_RADIUS, rgb_color_gray(), INFINITY);
            body_t *hole_bound = list_get(hole_elements, 0);
            body_set_centroid(hole_bound, vec_init(pos_x, pos_y));
            create_collision(scene, ball, hole_bound, level_end, scene, NULL);
            for (size_t j = 0; j < list_size(hole_elements); j++) {
                scene_add_body(scene, list_get(hole_elements, j));
            }
        }
        else if(strcmp(type, "GRASS") == 0) {
            body_t *grass = generate_grass(scene, ball, shape);
            scene_add_body(scene, grass);
        }
        else if(strcmp(type, "CIRCLE_GRASS") == 0) {
            cJSON *radius_p = cJSON_GetObjectItemCaseSensitive(object, "radius");
            double radius = radius_p->valuedouble;
            body_t *grass = generate_grass(scene, ball, create_circle_shape(radius));
            body_set_centroid(grass, vec_init(pos_x, pos_y));
            scene_add_body(scene, grass);
        }
        else if(strcmp(type, "WATER") == 0) {
            body_t *water = generate_water(scene, ball, shape);
            scene_add_body(scene, water);
        }
        else if(strcmp(type, "SAND") == 0) {
            body_t *sand = generate_sand(scene, ball, shape);
            scene_add_body(scene, sand);
        }
        else if(strcmp(type, "POWER") == 0) {
            body_t* powerup = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
            body_set_centroid(powerup, vec_init(pos_x, pos_y));
            scene_add_body(scene, powerup);
        }
        
        else if(strcmp(type, "SPIKE") == 0) {
            body_t *spike = generate_spike(scene, ball, create_triangle_shape(100.0));
            body_set_centroid(spike, vec_init(pos_x, pos_y));
            scene_add_body(scene, spike);
        }
    }
    goto end;

    end:
        free(data);
        cJSON_Delete(monitor_json);
}


// void generate_level1(scene_t *scene, body_t *ball) {
//     // body_t *gravity_source = get_gravity_body(scene);
//     // scene_add_body(scene, gravity_source);
//     // create_newtonian_gravity(scene, G, ball, gravity_source);
//     set_frame(scene, ball, scene_get_bound(scene));

//     scene_add_body(scene, generate_grass(scene, ball, generate_grass_shape()));
//     list_t *hole_elements = create_golf_hole(HOLE_RADIUS, rgb_color_gray(), INFINITY);
//     body_t *hole_bound = list_get(hole_elements, 0);
//     body_set_centroid(hole_bound, vec_init(LV1_HOLE_X, LV1_HOLE_Y));
//     create_collision(scene, ball, hole_bound, level_end, scene, NULL);
//     for (size_t j = 0; j < list_size(hole_elements); j++) {
//         scene_add_body(scene, list_get(hole_elements, j));
//     }

//     body_t *powerup = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
//     body_set_centroid(powerup, vec_init(1500, 500));
//     create_collision(scene, ball, powerup, power_up, scene, NULL);
//     scene_add_body(scene, powerup);
    
//     body_t *spike1 = generate_spike(scene, ball, create_triangle_shape(100.0));
//     body_set_centroid(spike1, vec_init(650, 275));
//     create_collision(scene, ball, spike1, level_end, scene, NULL);
//     scene_add_body(scene, spike1);

//     body_t *spike2 = generate_spike(scene, ball, create_triangle_shape(100.0));
//     body_set_centroid(spike2, vec_init(1200, 110));
//     create_collision(scene, ball, spike2, level_end, scene, NULL);
//     scene_add_body(scene, spike2);
// }

// void generate_level2(scene_t *scene, body_t *ball) {
//     body_set_centroid(ball, vec_init(50, 1000));

//     // body_t *gravity_source = get_gravity_body(scene);
//     // scene_add_body(scene, gravity_source);
//     // create_newtonian_gravity(scene, G, ball, gravity_source);
//     set_frame(scene, ball, scene_get_bound(scene));

//     list_t *water_s = create_rectangle_shape(1000, 280);
//     body_t *water_o = generate_water(scene, ball, water_s);
//     body_set_centroid(water_o, vec_init(400, 0));
//     scene_add_body(scene, water_o);
    
//     list_t *grass1_s = create_circle_shape(250);
//     body_t *grass1_o = generate_grass(scene, ball, grass1_s);
//     body_set_centroid(grass1_o, vec_init(70, 650));
//     scene_add_body(scene, grass1_o);

//     list_t *grass2_s = create_rectangle_shape(300, 700);
//     body_t *grass2_o = generate_grass(scene, ball, grass2_s);
//     body_set_centroid(grass2_o, vec_init(0, 0));
//     scene_add_body(scene, grass2_o);

//     list_t *grass3_s = list_init(3, free);
//     list_add(grass3_s, vec_init_ptr(300, 700));
//     list_add(grass3_s, vec_init_ptr(300, 0));
//     list_add(grass3_s, vec_init_ptr(800, 0));
//     body_t *grass3_o = generate_grass(scene, ball, grass3_s);
//     scene_add_body(scene, grass3_o);

//     list_t *grass4_s = list_init(3, free);
//     list_add(grass4_s, vec_init_ptr(1000, 0));
//     list_add(grass4_s, vec_init_ptr(1500, 400));
//     list_add(grass4_s, vec_init_ptr(1500, 0));
//     body_t *grass4_o = generate_grass(scene, ball, grass4_s);
//     scene_add_body(scene, grass4_o);

//     list_t *grass5_s = create_rectangle_shape(500, 400);
//     body_t *grass5_o = generate_grass(scene, ball, grass5_s);
//     body_set_centroid(grass5_o, vec_init(1500, 0));
//     scene_add_body(scene, grass5_o);

//     list_t *hole_elements = create_golf_hole(HOLE_RADIUS, rgb_color_gray(), INFINITY);
//     body_t *hole_bound = list_get(hole_elements, 0);
//     body_set_centroid(hole_bound, vec_init(1650, 400));

//     create_collision(scene, ball, hole_bound, level_end, scene, NULL);
//     for (size_t j = 0; j < list_size(hole_elements); j++) {
//         scene_add_body(scene, list_get(hole_elements, j));
//     }

//     body_t *powerup1 = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
//     body_set_centroid(powerup1,vec_init(500, 750));
//     create_collision(scene, ball, powerup1, power_up, scene, NULL);
//     scene_add_body(scene, powerup1);

//     body_t *powerup2 = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
//     body_set_centroid(powerup2,vec_init(1500, 1000));
//     create_collision(scene, ball, powerup2, power_up, scene, NULL);
//     scene_add_body(scene, powerup2);
// }

// void generate_level3(scene_t *scene, body_t *ball) {
//     body_set_centroid(ball, vec_init(50, 1000));

//     // body_t *gravity_source = get_gravity_body(scene);
//     // scene_add_body(scene, gravity_source);
//     // create_newtonian_gravity(scene, G, ball, gravity_source);
//     set_frame(scene, ball, scene_get_bound(scene));

//     list_t *sand_s = create_rectangle_shape(1000, 280);
//     body_t *water_o = generate_sand(scene, ball, sand_s);
//     body_set_centroid(water_o, vec_init(400, 0));
//     scene_add_body(scene, water_o);
    
//     list_t *grass1_s = create_circle_shape(300);
//     body_t *grass1_o = generate_grass(scene, ball, grass1_s);
//     body_set_centroid(grass1_o, vec_init(20, 590));
//     scene_add_body(scene, grass1_o);

//     list_t *grass2_s = create_rectangle_shape(300, 700);
//     body_t *grass2_o = generate_grass(scene, ball, grass2_s);
//     body_set_centroid(grass2_o, vec_init(0, 0));
//     scene_add_body(scene, grass2_o);

//     list_t *grass3_s = list_init(3, free);
//     list_add(grass3_s, vec_init_ptr(300, 700));
//     list_add(grass3_s, vec_init_ptr(300, 0));
//     list_add(grass3_s, vec_init_ptr(800, 0));
//     body_t *grass3_o = generate_grass(scene, ball, grass3_s);
//     scene_add_body(scene, grass3_o);

//     list_t *grass4_s = list_init(3, free);
//     list_add(grass4_s, vec_init_ptr(1000, 0));
//     list_add(grass4_s, vec_init_ptr(1500, 400));
//     list_add(grass4_s, vec_init_ptr(1500, 0));
//     body_t *grass4_o = generate_grass(scene, ball, grass4_s);
//     scene_add_body(scene, grass4_o);

//     list_t *grass5_s = create_rectangle_shape(500, 400);
//     body_t *grass5_o = generate_grass(scene, ball, grass5_s);
//     body_set_centroid(grass5_o, vec_init(1500, 0));
//     scene_add_body(scene, grass5_o);

//     list_t *hole_elements = create_golf_hole(HOLE_RADIUS, rgb_color_gray(), INFINITY);
//     body_t *hole_bound = list_get(hole_elements, 0);
//     body_set_centroid(hole_bound, vec_init(1650, 400));

//     create_collision(scene, ball, hole_bound, level_end, scene, NULL);
//     for (size_t j = 0; j < list_size(hole_elements); j++) {
//         scene_add_body(scene, list_get(hole_elements, j));
//     }
    
//     body_t *powerup1 = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
//     body_set_centroid(powerup1,vec_init(300, 1000));
//     create_collision(scene, ball, powerup1, power_up, scene, NULL);
//     scene_add_body(scene, powerup1);

//     body_t *powerup2 = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
//     body_set_centroid(powerup2,vec_init(500, 700));
//     create_collision(scene, ball, powerup2, power_up, scene, NULL);
//     scene_add_body(scene, powerup2);

//     body_t *powerup3 = generate_boost(scene, ball, create_nstar_shape(5, 50.0));
//     body_set_centroid(powerup3, vec_init(850, 1250));
//     create_collision(scene, ball, powerup3, power_up, scene, NULL);
//     scene_add_body(scene, powerup3);
// }


// // Looks pretty, can use again as another level
// // void set_frame(scene_t *scene, body_t *ball, vector_t size) {
// //     body_t *left = generate_grass(scene, ball, create_rectangle_shape(BUFFER, size.y + 2 * BUFFER));
// //     body_set_centroid(left, vec_init(-BUFFER / 2, size.y / 2));
// //     body_t *right = generate_grass(scene, ball, create_rectangle_shape(BUFFER, size.y + 2 * BUFFER));
// //     body_set_centroid(right, vec_init(size.x + BUFFER / 2, size.y / 2));

// //     body_t *top = generate_grass(scene, ball, create_rectangle_shape(size.x, BUFFER));
// //     body_set_centroid(top, vec_init(size.x / 2, size.y + BUFFER / 2));
// //     body_t *bottom = generate_grass(scene, ball, create_rectangle_shape(size.x, BUFFER));
// //     body_set_centroid(bottom, vec_init(size.x / 2, -BUFFER / 2));

// //     scene_add_body(scene, left);
// //     scene_add_body(scene, right);
// //     scene_add_body(scene, top);
// //     scene_add_body(scene, bottom);
// // }