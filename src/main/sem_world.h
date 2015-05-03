#ifndef SEM_WORLD_H
#define SEM_WORLD_H

typedef struct sem_world sem_world;
typedef struct sem_track sem_track;

#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_timer.h"
#include "sem_track_cache.h"
#include "sem_train.h"

typedef enum {
	BLANK, TRACK, POINTS, SIGNAL
} sem_tile_class;

typedef enum {
	MAIN_MANUAL, MAIN_AUTO, SUB
} sem_signal_type;

typedef enum {
	RED, AMBER, GREEN
} sem_signal_aspect;

struct sem_track {
	unit_vector start;
	unit_vector end;	
	sem_track* next; /**< \brief When two tracks appear on the same tile, this points to the second piece of track */
};

typedef struct {
	sem_signal_type type;
	sem_signal_aspect aspect;
} sem_signal;

typedef struct {
	sem_tile_class class;	
	sem_track* track;
	sem_track* points[3];
	sem_signal* signal;
} sem_tile;

struct sem_world {
	sem_timer_context* timer;
	sem_dynamic_array* trains;
	sem_dynamic_array* actions;
	uint32_t max_x;
	uint32_t max_y;
	sem_tile* tiles; // TODO: would be better not to have BLANK tiles, just have NULL pointers
	sem_track_cache* track_cache;
};

typedef struct {
	/**
	 * @brief The new direction for the head of the train.
	 */
	unit_vector direction;

	/**
	 * @brief The track which the head of the train must now occupy.
	 */
	sem_track* track;

	/**
	 * @brief Indicates whether or not the points need switching to make the track active to prevent derailment.
	 *
	 * If @c true, indicates that the train has been accepted onto the tile,
	 * but that the track on which it must run is not currently active.
	 * The train must either make that track active by
	 * calling sem_points_activate() or it must derail.
	 */
	bool need_points_switch;
} sem_tile_acceptance;

/**
 * @brief Initialise a world with \ref BLANK tiles, no trains and a default timer.
 *
 * Once initialised, a sem_world should be destroyed by calling sem_world_destroy().
 * 
 * @param world A pointer to an uninitialised sem_world whose dimensions are already specified.
 * @return \ref SEM_OK on success, or \ref SEM_ERROR if there was insufficient memory to initialise the world.
 */
sem_success sem_world_init_blank(sem_world* world);

/**
 * @brief Destroy an initialised world.
 *
 * The world's trains and tiles are freed.
 */
void sem_world_destroy(sem_world* world);

/**
 * @brief Add a train to the world
 * 
 * The train should already be initialised such that its cars are placed on 
 * the track belonging to the underlying tiles.
 *
 * @return \ref SEM_OK on success, or \ref SEM_ERROR if there was insufficient memory to add the train to the world.
 */
sem_success sem_world_add_train(sem_world* world, sem_train* train);

sem_tile* sem_tile_at_coord(sem_world* world, sem_coordinate* c);

sem_tile* sem_tile_at(sem_world* world, uint32_t x, uint32_t y);

/**
 * \brief Accept a train that has moved onto a new tile.
 *
 * @param acceptance Modified upon return to indicate the acceptance criteria.  If the call was not successful, the struct elements will be undefined.
 * @return \ref SEM_OK if the train was accepted onto active or inactive track, or \ref SEM_ERROR if the train moved onto disconnected train.
 */
sem_success sem_tile_accept(sem_train* train, sem_tile* tile, sem_tile_acceptance* acceptance);

/**
 * \brief Initialise a sem_tile_acceptance with default values.
 */
void sem_tile_acceptance_init(sem_tile_acceptance* acceptance);

void sem_tile_switch_points(sem_tile* tile);

void sem_tile_set_track(sem_tile* tile, sem_track* track);

/**
 * \brief Initialise a sem_tile with an active track, leaving the inactive tracks as NULL.
 */
void sem_tile_set_points(sem_tile* tile, sem_track* track);

void sem_tile_set_signal(sem_tile* tile, sem_track* track, sem_signal* signal);

/**
 * \brief Set track start and end points, and set next track to NULL.
 */
void sem_track_set(sem_track* track, unit_vector start, unit_vector end);

/**
 * \brief Find the piece of track matching the desired \ref key and return it in \arg destination.
 */
sem_success sem_track_matching(sem_tile* tile, sem_track* key, sem_track** destination);

#endif
