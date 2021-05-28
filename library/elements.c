#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "render.h"
#include "body.h"
#include "level_handlers.h"
#include "collision.h"
#include "forces.h"

list_t *create_golf_ball(double radius, rgb_color_t color, double mass) {
    list_t *golf_ball = list_init(3, (free_func_t) body_free);

    body_t *ball = create_circle(radius, mass);
    body_set_color(ball, color);
    list_add(golf_ball, ball);
    body_set_info(ball, make_type_info(BALL));

    body_t *backwing = create_lemniscate(3 * radius, M_PI / 2, 11 * M_PI / 12, mass);
    body_set_color(backwing, rgb_color_gray());
    list_add(golf_ball, backwing);
    body_add_anchor(ball, backwing);

    body_t *frontwing = create_lemniscate(3 * radius, 5 * M_PI / 12, 3 * M_PI / 2, mass);
    body_set_color(frontwing, rgb_color_gray());
    list_add(golf_ball, frontwing);
    body_add_anchor(ball, frontwing);
    vector_t starting_placement = {.x = 0, .y = 300};

    for(size_t i = 0; i < list_size(golf_ball); i++) {
        body_set_centroid(list_get(golf_ball, i), starting_placement);
    }
    return golf_ball;
}

list_t *create_golf_hole(double radius, rgb_color_t color, double mass) {
    list_t *golf_hole = list_init(3, (free_func_t) body_free);

    body_t *hole = create_circle(radius, mass);
    body_set_color(hole, color);
    list_add(golf_hole, hole);
    body_set_info(hole, make_type_info(HOLE));

    body_t *flagpole = create_rectangle(radius / 10, 2 * radius, 0);
    body_set_color(flagpole, rgb_color_init(0, 0, 0));
    body_set_centroid(flagpole, (vector_t) {radius, radius});
    body_add_anchor(hole, flagpole);

    body_t *flag = create_nstar(3, radius / 4, 0);
    body_set_color(flag, rgb_color_red());
    body_set_centroid(flag, (vector_t) {11 / 10 * radius, 2 * radius});
    list_add(golf_hole, flag);
    body_add_anchor(hole, flag);

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
