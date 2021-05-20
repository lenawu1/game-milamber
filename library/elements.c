#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "render.h"
#include "compound_body.h"

comp_body_t *create_golf_ball(double rad, rgb_color_t col, double mass) {
    comp_body_t *golf_ball = comp_body_init();
    
    body_t *ball = create_circle(rad, mass);
    body_set_color(ball, col);
    comp_body_add(golf_ball, ball);

    body_t *backwing = create_lemniscate(3 * rad, M_PI / 2, 11 * M_PI / 12, mass);
    body_set_color(backwing, rgb_color_gray());
    comp_body_add(golf_ball, backwing); //TODO: Consider relative centroid positioning of wings

    body_t *frontwing = create_lemniscate(3 * rad, 5 * M_PI / 12, 3 * M_PI / 2, mass);
    body_set_color(frontwing, rgb_color_gray());
    comp_body_add(golf_ball, frontwing); //TODO: Consider relative centroid positioning of wings

    return golf_ball;
}

/**
 * Creates a colored golf ball with wings and mass.
 *
 * @param rad the radius of the target hole
 * @param col the RGB color of the hole, normally black
 * @param mass the hole's mass, can simulate a slope towards it
 * @return a pointer to the newly created golf hole compound body
 */
comp_body_t *create_golf_hole(double rad, rgb_color_t col, double mass) {
    comp_body_t *golf_hole = comp_body_init();

    body_t *hole = create_circle(rad, mass);
    body_set_color(hole, col);
    comp_body_add(golf_hole, hole);

    body_t *flagpole = create_rectangle(rad / 10, 2 * rad, 0);
    body_set_color(flagpole, rgb_color_init(0, 0, 0));
    comp_body_add(golf_hole, flagpole); //TODO: Consider relative centroid positioning of pole

    body_t *flag = create_nstar(3, rad / 4, 0);
    body_set_color(flag, rgb_color_red());
    comp_body_add(golf_hole, flag); //TODO: Consider relative centroid positioning of flag

    return golf_hole;
}