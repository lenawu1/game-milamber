#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include "list.h"
#include "vector.h"
#include "sdl_wrapper.h"
#include "scene.h"
#include "forces.h"
#include "body.h"
#include "color.h"
#include "render.h"
#include "collision.h"
#include "polygon.h"
#include "compound_body.h"
#include "elements.h"
#include "physics.h"
#include "terrain.h"
#include "level_handlers.h"

// const double SCREEN_SIZE_Y = 750;
// const double SCREEN_SIZE_X = 750;
const vector_t SCREEN_SIZE = {.x = 1800, .y = 500};
const vector_t PLAYER_SPEED = {.x = 200, .y = 500};
const double BALL_MASS = 40.0;
const double MASS = 10;
const double RADIUS = 10;
const double BALL_SIZE = 6;
const double HOLE_SIZE = 10;
const double PLAYER_POS = 20;
const double LARGE_MASS = INFINITY;
const double WALL_THICKNESS = 10;
const double TRAIL_SIZE = 6;
const double GRAV_VAL = 1200;
// const rgb_color_t PORTAL_COLOR = {.R = 170, .G = 0, .B = 255}; just to remember the color
size_t LEVEL = 1;

void handler(char key, key_event_type_t type, double held_time, scene_t *scene) {
    body_t *golfball = scene_get_body(scene, 0);
    if (type == KEY_PRESSED) {
        if (key == RIGHT_ARROW) {
            vector_t right_v = PLAYER_SPEED;
            body_set_velocity(golfball, right_v);
            scene_add_point(scene);
        }
        else if (key == LEFT_ARROW) {
            vector_t left_v = {.x = -1.0*PLAYER_SPEED.x, .y = PLAYER_SPEED.y};
            body_set_velocity(golfball, left_v);
            scene_add_point(scene);
        }
    }
}


int main(int argc, char *argv[]) {
    sdl_init(VEC_ZERO, SCREEN_SIZE);
    scene_t *scene = scene_init();
    list_t *ball_elements = create_golf_ball(RADIUS, rgb_color_init(205, 99, 75), BALL_MASS);

    body_t *player = list_get(ball_elements, 0);

    for(size_t i = 0; i < list_size(ball_elements); i++) {
        scene_add_body(scene, list_get(ball_elements, i));
    }
    generate_level1(scene, player);
    sdl_on_key(handler,scene);
    
    double clock = 0.0;
    
    while (!sdl_is_done(scene) && scene_get_level(scene) == 1 && scene_get_state(scene) == 0){
        double dt = time_since_last_tick();
        clock += dt;

        //double curr_points = (double)scene_get_points(scene);
        //point_display(curr_points);
        do_gravity(player, GRAV_VAL, dt);
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    int state = scene_get_state(scene);

    //TODO: What to do if ball goes out of bounds? do they lose?

    if (state == -1) {
        printf("Sorry, you lost! \n");        
    }
    else if (state == 1) {
        scene_add_level(scene);
        printf("Good job! You won this level. \n");        
    }

    scene_free(scene);
}