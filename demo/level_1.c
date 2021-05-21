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
#include "terrain.h"

const double SCREEN_SIZE_Y = 750;
const double SCREEN_SIZE_X = 750;
const vector_t SCREEN_SIZE = {.x = 750, .y = 750};
const vector_t PLAYER_SPEED = {.x = 700, .y = 700};
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

void bounce(comp_body_t *golfball, double held_time, vector_t init_v) {
    vector_t last_v = comp_body_get_velocity(golfball);
    vector_t with_gravity = vec_subtract(last_v, vec_init(0, held_time * GRAVITY));
    body_set_velocity(golfball, with_gravity);
}

// may not need this if we have collision between terrain and ball
bool reaches_bottom(comp_body_t *golfball) {
    body_t *ball = list_get(get_internal_bodies(golfball), 0);
    list_t *ball_points = body_get_shape(ball);
    for (size_t i = 0; i < list_size(ball_points); i++) {
        double y = ((vector_t *) list_get(ball_points, i))->y;
        //TODO: replace 0 with the y of the terrain/ground.
        if (y <= 0 && vec_norm(body_get_velocity(ball)) < 0) {
            return true;
        }
    }
    return false;
}

// TODO: handler of the scene keyboard manipulation
void handler(char key, key_event_type_t type, double held_time, scene_t *scene) {
    comp_body_t *golfball = scene_get_body(scene, 0);
    if (type == KEY_PRESSED) {
        held_time += 1.5;
        if (key == RIGHT_ARROW) {
            vector_t right_v = {.x = PLAYER_SPEED.x, .y = PLAYER_SPEED.y};
            bounce(golfball, held_time, right_v);
        }
        else if (key == LEFT_ARROW) {
            vector_t left_v = {.x = -1.0 * PLAYER_SPEED.x, .y = PLAYER_SPEED.y};
            bounce(golfball, held_time, left_v);
        }
    }
    else { // this def needs fixing
        vector_t last_v = comp_body_get_velocity(golfball);
        body_set_velocity(golfball, vec_subtract(last_v, vec_init(0, GRAVITY)));
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
        //player_in_bounds(scene);
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
}