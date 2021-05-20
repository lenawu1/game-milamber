#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "render.h"
#include "compound_body.h"

bool level_end(comp_body_t *ball, comp_body_t *hole) {
    if (list_contains(body_get_collisions(ball), hole)) {
        return true;
    }
    return false;
}