#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "render.h"
#include "compound_body.h"

/**
 * Marks a whether the level goal is achieved, at which point the level should end.
 *
 * @param ball the golf ball
 * @param hole the hole it should reach
 * @return a level number to proceed with
 */
bool level_end(comp_body_t *ball, comp_body_t *hole);

/**
 * Generates sand that will have no elasticity and bring the speed of the ball to zero.
 * Use with scene_add_body to add to scene.
 *
 * @param scene the scene in which the sand should be placed
 * @param ball the golf ball
 * @param shape the shape of the sand terrain, list of vectors
 * @return a comp_body_t of the sand terrain that can be added to the scene
 */
comp_body_t *generate_sand(scene_t *scene, comp_body_t *ball, list_t *shape);


#endif // ifndef __TERRAIN_H__