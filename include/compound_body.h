#ifndef __COMPOUND_BODY_H__
#define __COMBOUND_BODY_H__

#include <stdbool.h>
#include "body.h"

/**
 * A compound body constrained to the plane.
 * This body is composed of multiple bodies that act as one.
 */
typedef struct compound_body comp_body_t;

/**
 * Initializes a compound body without any info.
 * Allocates the required memory.
 */
comp_body_t *comp_body_init();

/**
 * Releases the memory allocated for a compound body and the bodies within it.
 *
 * @param comp_body a pointer to a body returned from comp_body_init()
 */
void comp_body_free(comp_body_t *comp_body);

/**
 * Adds a composing body to the compound body.
 *
 * @param comp_body a pointer to a body returned from comp_body_init()
 */
void comp_body_add(comp_body_t *comp_body, body_t *body);

/**
 * Returns a list of the bodies that compose this compound body.
 *
 * @param comp_body a pointer to a body returned from comp_body_init()
 */
list_t *get_internal_bodies(comp_body_t *comp_body);

bool comp_body_collided(comp_body_t *comp_body);

void comp_body_set_collided(comp_body_t *comp_body, bool val);

/**
 * Gets the current center of mass of the main body, first in the list.
 *
 * @param comp_body a pointer to a body returned from comp_body_init()
 * @return the main body's center of mass
 */
vector_t comp_body_get_centroid(comp_body_t *comp_body);

/**
 * Gets the current velocity of a body.
 *
 * @param comp_body a pointer to a body returned from comp_body_init()
 * @return the compound body's velocity vector
 */
vector_t comp_body_get_velocity(comp_body_t *comp_body);

/**
 * Gets the mass of the compound body.
 * This is the sum of all composing bodies
 *
 * @param comp_body a pointer to a body returned from comp_body_init()
 * @return the total mass of the compound body.
 */
double comp_body_get_mass(comp_body_t *comp_body);

/**
 * Translates a compound body to a new position.
 * The position is specified by the position of the main body's center of mass.
 *
 * @param comp_body a pointer to a body returned from comp_body_init()
 * @param x the body's new centroid
 */
void comp_body_set_centroid(comp_body_t *comp_body, vector_t x);

/**
 * Changes a compound body's velocity (the time-derivative of its position).
 *
 * @param comp_body a pointer to a body returned from comp_body_init()
 * @param v the compound body's new velocity
 */
void comp_body_set_velocity(comp_body_t *comp_body, vector_t v);

/**
 * Changes a compound body's orientation in the plane.
 * The compound body is rotated about its center of mass.
 * Note that the angle is *absolute*, not relative to the current orientation.
 *
 * @param comp_body a pointer to a body returned from comp_body_init()
 * @param angle the body's new angle in radians. Positive is counterclockwise.
 */
void comp_body_set_rotation(comp_body_t *comp_body, double angle);

double comp_body_get_rotation(comp_body_t *comp_body);
/**
 * Applies a force to each composing body.
 *
 * @param comp_body a pointer to a body returned from comp_body_init()
 * @param force the force vector to apply
 */
void comp_body_add_force(comp_body_t *comp_body, vector_t force);

/**
 * Applies an impulse to each composing body.
 *
 * @param comp_body a pointer to a body returned from comp_body_init()
 * @param impulse the impulse vector to apply
 */
void comp_body_add_impulse(comp_body_t *comp_body, vector_t impulse);

/**
 * Ticks every composing body.
 *
 * @param comp_body the compound body to tick
 * @param dt the number of seconds elapsed since the last tick
 */
void comp_body_tick(comp_body_t *comp_body, double dt);

/**
 * Marks each composing body for removal, and stores this.
 * Does not free the body.
 * If the body is already marked for removal, does nothing.
 *
 * @param comp_body the compound body to mark for removal
 */
void comp_body_remove(comp_body_t *comp_body);

/**
 * Returns whether a compound body and its composing bodies have been marked for removal.
 * This function returns false until comp_body_remove() is called on the body,
 * and returns true afterwards.
 *
 * @param comp_body the compound body to mark for removal
 * @return whether comp_body_remove() has been called on the body
 */
bool comp_body_is_removed(comp_body_t *comp_body);

#endif // ifndef __COMPOUND_BODY_H__