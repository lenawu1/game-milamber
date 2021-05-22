#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "render.h"
#include "compound_body.h"

comp_body_t *create_golf_ball(double radius, rgb_color_t color, double mass) {
    comp_body_t *golf_ball = comp_body_init();
    
    body_t *ball = create_circle(radius, mass);
    body_set_color(ball, color);
    comp_body_add(golf_ball, ball);

    body_t *backwing = create_lemniscate(3 * radius, M_PI / 2, 11 * M_PI / 12, mass);
    body_set_color(backwing, rgb_color_gray());
    comp_body_add(golf_ball, backwing);

    body_t *frontwing = create_lemniscate(3 * radius, 5 * M_PI / 12, 3 * M_PI / 2, mass);
    body_set_color(frontwing, rgb_color_gray());
    comp_body_add(golf_ball, frontwing);

    return golf_ball;
}

comp_body_t *create_golf_hole(double radius, rgb_color_t color, double mass) {
    comp_body_t *golf_hole = comp_body_init();

    body_t *hole = create_circle(radius, mass);
    body_set_color(hole, color);
    comp_body_add(golf_hole, hole);

    body_t *flagpole = create_rectangle(radius / 10, 2 * radius, 0);
    body_set_color(flagpole, rgb_color_init(0, 0, 0));
    body_set_centroid(flagpole, (vector_t) {radius, radius});
    comp_body_add(golf_hole, flagpole);

    body_t *flag = create_nstar(3, radius / 4, 0);
    body_set_color(flag, rgb_color_red());
    body_set_centroid(flag, (vector_t) {11 / 10 * radius, 2 * radius});
    comp_body_add(golf_hole, flag);

    return golf_hole;
}