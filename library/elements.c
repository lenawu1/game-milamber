#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "elements.h"
#include "render.h"
#include "sdl_wrapper.h"
#include "body.h"
#include "level_handlers.h"
#include "collision.h"
#include "forces.h"

const rgb_color_t FLAG_COLOR = (rgb_color_t) {1.0, 0.549, 0.0};

list_t *create_golf_ball(double radius, rgb_color_t color, double mass, vector_t location) {
    list_t *golf_ball = list_init(1, (free_func_t) body_free);
    body_t *ball = create_circle(radius, mass);
    SDL_Texture *ball_tex = sdl_load_texture("../resources/pixel_ball.png");
    body_set_texture(ball, ball_tex);

    body_set_color(ball, color);
    list_add(golf_ball, ball);
    body_set_info(ball, make_type_info(BALL));
    body_set_centroid(ball, location);

    return golf_ball;
}

list_t *create_golf_hole(double radius, rgb_color_t color, double mass) {
    list_t *golf_hole = list_init(3, (free_func_t) body_free);

    body_t *hole = create_circle(radius, mass);
    SDL_Texture *hole_tex = sdl_load_texture("../resources/hole_sprite.png");
    body_set_texture(hole, hole_tex);
    list_add(golf_hole, hole);
    body_set_info(hole, make_type_info(HOLE));

    body_t *flagpole = create_rectangle(radius / 5, 3 * radius, INFINITY);
    body_set_color(flagpole, rgb_color_init(0.2, 0.2, 0.2));
    body_set_centroid(flagpole, (vector_t) {0, radius});
    list_add(golf_hole, flagpole);
    body_add_anchor(hole, flagpole);
    body_set_info(flagpole, make_type_info(HOLE));

    body_t *flag = create_nstar(3, radius, INFINITY);
    SDL_Texture *flag_tex = sdl_load_texture("../resources/flag_sprite.png");
    body_set_texture(flag, flag_tex);
    body_set_rotation(flag, M_PI / 6);
    body_set_centroid(flag, (vector_t) {0.6*radius, 3.2 * radius});
    list_add(golf_hole, flag);
    body_add_anchor(hole, flag);
    body_set_info(flag, make_type_info(HOLE));

    return golf_hole;
}

list_t *create_portals(
    scene_t *scene,
    body_t *ball,
    double radius,
    double mass,
    rgb_color_t color,
    vector_t coord1,
    vector_t coord2
) {
    list_t *portals = list_init(2, (free_func_t) body_free);

    body_t *portal1 = create_circle(radius, mass);
    body_set_color(portal1, color);
    body_set_centroid(portal1, coord1);
    list_add(portals, portal1);

    body_t *portal2 = create_circle(radius, mass);
    body_set_color(portal2, color);
    body_set_centroid(portal2, coord1);
    list_add(portals, portal2);

    body_set_info(portal1, portal2);
    body_set_info(portal2, portal1);
    
    create_collision(scene, ball, portal1, teleport, NULL, NULL);

    return portals;
}
