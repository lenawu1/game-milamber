#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include "assert.h"
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
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_gfxPrimitives.h>


// const double SCREEN_SIZE_Y = 750;
// const double SCREEN_SIZE_X = 750;
const vector_t SCREEN_SIZE = {.x = 2000, .y = 1000};
const vector_t PLAYER_SPEED = {.x = 1000, .y = 1000};
// const vector_t INIT_POS = {.x = 50, .y = 1000};

// const double BALL_MASS = 40.0;
const double MASS = 10;
const double RADIUS = 10;
// const double BALL_SIZE = 20;
const double PLAYER_POS = 20;
const double LARGE_MASS = INFINITY;
const double WALL_THICKNESS = 10;
const double TRAIL_SIZE = 6;
const double GRAV_VAL = 3500;
// const rgb_color_t PORTAL_COLOR = {.R = 170, .G = 0, .B = 255}; just to remember the color
size_t LEVEL = 1;

void handler(char key, key_event_type_t type, double held_time, scene_t *scene) {
    body_t *golfball = scene_get_body(scene, 0);
    char *filepath = "../resources/popsound.wav";
    if (type == KEY_PRESSED) {
        if (key == RIGHT_ARROW) {
            if (scene_get_state(scene) == 0) {
                vector_t right_v = PLAYER_SPEED;
                body_translate(golfball, vec_init(0, 10));
                body_set_velocity(golfball, right_v);
                scene_add_point(scene);
                sdl_load_sound(filepath);
            }
        }
        else if (key == LEFT_ARROW) {
            if (scene_get_state(scene) == 0) {
                vector_t left_v = {.x = -1.0*PLAYER_SPEED.x, .y = PLAYER_SPEED.y};
                body_translate(golfball, vec_init(0, 10));
                body_set_velocity(golfball, left_v);
                scene_add_point(scene);
                sdl_load_sound(filepath);
            }
        }
        else if (key == UP_ARROW) {
            if (scene_get_level(scene) == 5) {
                scene_set_state(scene, 2);
            }
            else {
                if(scene_get_state(scene) == 1) {
                    scene_add_level(scene);
                    // build_level(scene);
                    scene_set_points(scene, 0);
                    body_set_velocity(golfball, VEC_ZERO);
                }
                scene_set_state(scene, 0); // Continue playing
            }
        }
        else if (key == SPACE) {
            if(scene_get_state(scene) == -1 || 1)
            {
                if(scene_get_level(scene) == 1)
                {
                    scene_set_level(scene,1);
                    reset_scene(scene);
                    build_level(scene);
                    scene_set_points(scene, 0);
                    body_set_velocity(golfball, VEC_ZERO);
                }
                else
                {
                    scene_set_level(scene, scene_get_level(scene) - 1); // decrement scene
                    scene_add_level(scene);
                    scene_set_points(scene, 0);
                    body_set_velocity(golfball, VEC_ZERO);
                }
            }
            scene_set_state(scene, 0); // Continue playing
        }
        else if (key == Q_CHARACTER) {
            //scene_set_state(scene, -2);
            SDL_Quit();
            exit(0);
        }
        /*
        
        */
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

    body_t *player = build_level(scene);
    sdl_on_key(handler, scene);
    // center_display("starting");
    double clock = 0.0;
    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        clock += dt;
        do_gravity(player, GRAV_VAL, dt);
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    scene_free(scene);
}
