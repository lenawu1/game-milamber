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

// Game level 1 here

const double SCREEN_SIZE_Y = 750;
const double SCREEN_SIZE_X = 750;
const vector_t SCREEN_SIZE = {.x = 750, .y = 750};
const double PLAYER_SPEED = 700;
const double MASS = 10.0;
const double RADIUS = 30;
const double BALL_SIZE = 6;
const double HOLE_SIZE = 10;
const double PLAYER_POS = 20;
const double LARGE_MASS = INFINITY;
const double WALL_THICKNESS = 10;

size_t LEVEL = 1;

// TODO: handler of the scene keyboard manipulation

int main(int argc, char *argv[]) {
    sdl_init(VEC_ZERO, SCREEN_SIZE);

    scene_t *scene = scene_init();
    comp_body_t *player = create_golf_ball();
    comp_body_t *hole = create_golf_hole();

    // TODO
    // generate_walls(scene, ball);

    // sdl_on_key(handler,scene);
    
    double clock = 0.0;
    
    while (!sdl_is_done(scene, NULL) && LEVEL == 1){
        double dt = time_since_last_tick();
        clock += dt;

        if (level_end(player, hole)) {
            for (size_t i = 1; i < scene_bodies(scene); i++) {
                body_remove(scene_get_body(scene, i));
            }
            clock = 0;
            LEVEL++;
        }
        player_in_bounds(scene);
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
}