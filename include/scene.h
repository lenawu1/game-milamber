#ifndef __SCENE_H__
#define __SCENE_H__

#include "body.h"
#include "list.h"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

/**
 * A collection of bodies and force creators.
 * The scene automatically resizes to store
 * arbitrarily many bodies and force creators.
 */
typedef struct scene scene_t;

 typedef struct force force_bundle_t;

/**
 * A function which adds some forces or impulses to bodies,
 * e.g. from collisions, gravity, or spring forces.
 * Takes in an auxiliary value that can store parameters or state.
 */
typedef void (*force_creator_t)(void *aux, list_t *list);

/**
 * Allocates memory for an empty scene.
 * Makes a reasonable guess of the number of bodies to allocate space for.
 * Asserts that the required memory is successfully allocated.
 *
 * @return the new scene
 */
scene_t *scene_init(void);

/**
 * Releases memory allocated for a given scene
 * and all the bodies and force creators it contains.
 *
 * @param scene a pointer to a scene returned from scene_init()
 */
void scene_free(scene_t *scene);

/**
 * Gets the number of bodies in a given scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @return the number of bodies added with scene_add_body()
 */
size_t scene_bodies(scene_t *scene);

void scene_add_sound(scene_t *scene, Mix_Chunk *sound);

size_t scene_background_elements(scene_t *scene);

body_t *scene_get_background_element(scene_t *scene, size_t index);

void scene_add_background_element(scene_t *scene, body_t *body);

list_t *scene_get_force_bundles(scene_t *scene);

force_creator_t get_force_bundle_forcer(force_bundle_t *force_bundle);

list_t *get_force_bundle_bodies(force_bundle_t *force_bundle);

size_t scene_get_points(scene_t *scene);

size_t scene_get_level(scene_t *scene);

void reset_scene(scene_t *scene);

bool scene_set_first_try(scene_t *scene, bool retry_first_level);

void scene_set_img(scene_t *scene, SDL_Texture *img);

SDL_Texture *scene_get_img(scene_t *scene);

bool scene_get_first_try(scene_t *scene);

int scene_get_state(scene_t *scene);

vector_t scene_get_bound(scene_t *scene);

/**
 * Gets the body at a given index in a scene.
 * Asserts that the index is valid.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param index the index of the body in the scene (starting at 0)
 * @return a pointer to the body at the given index
 */
body_t *scene_get_body(scene_t *scene, size_t index);

/**
 * Adds a body to a scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param body a pointer to the body to add to the scene
 */
void scene_add_body(scene_t *scene, body_t *body);

/**
 * Increases the point in the scene by 1.
 *
 * @param scene a pointer to a scene returned from scene_init()
 */
size_t scene_add_point(scene_t *scene);

size_t scene_set_points(scene_t *scene, size_t point);

/**
 * Increases the level in the scene by 1.
 *
 * @param scene a pointer to a scene returned from scene_init()
 */
size_t scene_add_level(scene_t *scene);

int scene_set_state(scene_t *scene, int state);

size_t scene_set_level(scene_t *scene, size_t level);

/**
 * @deprecated Use body_remove() instead
 *
 * Removes and frees the body at a given index from a scene.
 * Asserts that the index is valid.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param index the index of the body in the scene (starting at 0)
 */
void scene_remove_body(scene_t *scene, size_t index);

/**
 * @deprecated Use scene_add_bodies_force_creator() instead
 * so the scene knows which bodies the force creator depends on
 */
void scene_add_force_creator(
    scene_t *scene,
    force_creator_t forcer,
    void *aux,
    free_func_t freer
);

/**
 * Adds a force creator to a scene,
 * to be invoked every time scene_tick() is called.
 * The auxiliary value is passed to the force creator each time it is called.
 * The force creator is registered with a list of bodies it applies to,
 * so it can be removed when any one of the bodies is removed.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param forcer a force creator function
 * @param aux an auxiliary value to pass to forcer when it is called
 * @param bodies the list of bodies affected by the force creator.
 *   The force creator will be removed if any of these bodies are removed.
 *   This list does not own the bodies, so its freer should be NULL.
 * @param freer if non-NULL, a function to call in order to free aux
 */
void scene_add_bodies_force_creator(
    scene_t *scene,
    force_creator_t forcer,
    void *aux,
    list_t *bodies,
    free_func_t freer
);

/**
 * Executes a tick of a given scene over a small time interval.
 * This requires executing all the force creators
 * and then ticking each body (see body_tick()).
 * If any bodies are marked for removal, they should be removed from the scene
 * and freed, along with any force creators acting on them.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param dt the time elapsed since the last tick, in seconds
 */
void scene_tick(scene_t *scene, double dt);

#endif // #ifndef __SCENE_H__
