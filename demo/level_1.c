#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
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

const double SCREEN_SIZE_Y = 750;
const double SCREEN_SIZE_X = 750;
const vector_t SCREEN_SIZE = {.x = 750, .y = 750};
const vector_t PLAYER_SPEED = {.x = 500, .y = 1000};
const double BALL_MASS = 30.0;
const double MASS = 10;
const double RADIUS = 10;
const double BALL_SIZE = 6;
const double HOLE_SIZE = 10;
const double PLAYER_POS = 20;
const double LARGE_MASS = INFINITY;
const double WALL_THICKNESS = 10;
const double GRAVITY = 7500;
const double TRAIL_SIZE = 6;
size_t LEVEL = 1;

// TODO: handler of the scene keyboard manipulation
void handler(char key, key_event_type_t type, double held_time, scene_t *scene) {
    comp_body_t *golfball = scene_get_body(scene, 0);
    if (type == KEY_PRESSED) {
            if (key == RIGHT_ARROW) {
            vector_t right_v = PLAYER_SPEED;
            comp_body_set_velocity(golfball, right_v);
        }
        else if (key == LEFT_ARROW) {
            vector_t left_v = {.x = -1.0*PLAYER_SPEED.x, .y = PLAYER_SPEED.y};
            comp_body_set_velocity(golfball, left_v);
        }
    }
}

/*
void trail(comp_body_t *golfball)
{
    
}
*/

int main(int argc, char *argv[]) {
    sdl_init(VEC_ZERO, SCREEN_SIZE);
    scene_t *scene = scene_init();
    comp_body_t *player = create_golf_ball(RADIUS, rgb_color_yellow(), BALL_MASS);
    scene_add_body(scene, player);
    comp_body_t *hole = create_golf_hole(RADIUS+2, rgb_color_gray(), MASS);
    scene_add_body(scene, hole);

    // TODO
    // generate_walls(scene, ball);

    // sdl_on_key(handler,scene);
    
    double clock = 0.0;
    
    while (!sdl_is_done(scene) && LEVEL == 1){
        double dt = time_since_last_tick();
        clock += dt;

        if (level_end(player, hole)) {
            for (size_t i = 1; i < scene_bodies(scene); i++) {
                body_remove(scene_get_body(scene, i));
            }
            clock = 0;
            LEVEL++;
        }

        
        do_gravity(player, GRAVITY, dt);
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
}