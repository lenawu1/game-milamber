#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "sdl_wrapper.h"
#include "polygon.h"
#include "render.h"
#include "physics.h"
#include "color.h"
#include "body.h"

const double INIT_X_VEL = 50;
const double INIT_Y_VEL = 0;
const double ANG_SPEED = 8;
const double SIZE = 6;
const double ELAS = 0.8; // elasticity
const int N = 8; // max number of shapes on screen
const int MAX_VEX = 10; // maximum number of vertices
const int MIN_VEX = 2; // minimum number of vertices
const vector_t WINDOW_MAX = {.x = 100, .y = 50};
const vector_t INIT_POS = {.x = 10, .y = 40};
const double GRAVITY = 100;
const double MASS = 0.1;

double render_delay(double x_vel, double size, double elasticity) {
    return 2.5 * (size / x_vel);
}

void add_shape(list_t *shapes){
    int vertices = (rand() % (MAX_VEX - MIN_VEX + 1)) + MIN_VEX;
    body_t *new_shape = create_nstar(vertices, SIZE, MASS);
    vector_t vel = {.x = INIT_X_VEL, .y = INIT_Y_VEL};
    body_set_centroid(new_shape, INIT_POS);
    body_set_velocity(new_shape, vel);
    list_add(shapes, new_shape);
}

void update_shapes(list_t *shapes, double dt){ // Updates positions depending on collisions
    if(list_size(shapes) == 0){
        return;
    }
    for(size_t i = 0; i < list_size(shapes); i++){
        body_t *star = list_get(shapes, i);
        
        star = do_gravity(star, GRAVITY, dt);
        double angle = body_get_rotation(star) + ANG_SPEED * dt;
        body_set_rotation(star, angle);

        star = collide(star, VEC_ZERO, vec_multiply(2.0, WINDOW_MAX), ELAS);
        
        list_t *shape = body_get_shape(star);
        if(check_offscreen(shape, VEC_ZERO, WINDOW_MAX)){
            list_remove(shapes, i);
        }
        body_tick(star, dt);
    }
}

void draw_shape(body_t *shape) {
    sdl_draw_polygon(body_get_shape(shape),
                     (rgb_color_t) {body_get_color(shape).r, body_get_color(shape).g, body_get_color(shape).b});
}

void draw_shapes(list_t *shapes){
    sdl_clear();
    if(list_size(shapes) == 0){
        return;
    }
    for(size_t i = 0; i < list_size(shapes); i++){ // Iterate through list and draws each shape
        draw_shape(list_get(shapes, i));
    }
    sdl_show();
}

void free_mem(list_t *shapes){
    for(size_t i = 0; i < list_size(shapes); i++){
        list_free(list_get(shapes, i));
    }
    list_free(shapes);
}

int main(int argc, char *argv[]) {
    // Create star at initial positions
    list_t *shapes = list_init(N, (free_func_t) body_free);
    sdl_init(VEC_ZERO, WINDOW_MAX);

    double t = 0.0;
    add_shape(shapes);
    while(!sdl_is_done(NULL, NULL)){
        double dt = time_since_last_tick();
        t += dt;
        if(t > render_delay(INIT_X_VEL, SIZE, ELAS) && list_size(shapes) < N){
            add_shape(shapes);
            t = 0;
        }
        update_shapes(shapes, dt);
        draw_shapes(shapes);
    }
    
    list_free(shapes);
}
