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

#endif // ifndef __TERRAIN_H__