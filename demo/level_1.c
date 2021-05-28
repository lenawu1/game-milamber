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

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>


// const double SCREEN_SIZE_Y = 750;
// const double SCREEN_SIZE_X = 750;
const vector_t SCREEN_SIZE = {.x = 2000, .y = 1000};
const vector_t PLAYER_SPEED = {.x = 1500, .y = 1500};
const vector_t INIT_POS = {.x = 50, .y = 1000};

const double BALL_MASS = 40.0;
const double MASS = 10;
const double RADIUS = 10;
const double BALL_SIZE = 20;
const double PLAYER_POS = 20;
const double LARGE_MASS = INFINITY;
const double WALL_THICKNESS = 10;
const double TRAIL_SIZE = 6;
const double GRAV_VAL = 7500;
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

// void update_text(scene_t *scene) {
//     TTF_Font *font = TTF_OpenFont("resources/OpenSans-Regular.ttf", 12);
//     SDL_Color textColor = {0, 0, 0, 255}; // Black
//     SDL_Color textBackgroundColor = {255, 255, 255, 255}; // White
//     char score_str[10];
//     SDL_Surface *textSurface = TTF_RenderText_Shaded(font, itoa(scene_get_points(scene), score_str, 10),
//                                                      textColor, textBackgroundColor);
//     SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, textSurface);
// }

int main(int argc, char *argv[]) {
    sdl_init(VEC_ZERO, SCREEN_SIZE);
    
    scene_t *scene = scene_init();

    list_t *ball_elements = create_golf_ball(BALL_SIZE, rgb_color_init(205, 99, 75), BALL_MASS, INIT_POS);
    body_t *player = list_get(ball_elements, 0);

    for(size_t i = 0; i < list_size(ball_elements); i++) {
        scene_add_body(scene, list_get(ball_elements, i));
    }
    generate_level1(scene, player);
    sdl_on_key(handler, scene);
    // center_display("starting");
    double clock = 0.0;
    
    while (!sdl_is_done(scene) && scene_get_level(scene) == 1 && scene_get_state(scene) == 0){
        double dt = time_since_last_tick();
        clock += dt;
        // char score_str[10];
        // sprintf(score_str, "%zu", scene_get_points(scene));
        // point_display(score_str);
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