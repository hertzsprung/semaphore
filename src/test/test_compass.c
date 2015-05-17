#include <glib.h>

#include "test_compass.h"
#include "sem_compass.h"

void test_compass_opposite_south_east(void);
void test_compass_opposite_north_west(void);
void test_compass_straight(void);
void test_compass_not_straight(void);
void test_compass_corner_of_curve_sw_e(void);

void add_tests_compass() {
	g_test_add_func("/compass/opposite_south_east", test_compass_opposite_south_east);
	g_test_add_func("/compass/opposite_north_west", test_compass_opposite_north_west);
	g_test_add_func("/compass/straight", test_compass_straight);
	g_test_add_func("/compass/not_straight", test_compass_not_straight);
	g_test_add_func("/compass/corner_of_curve_sw_e", test_compass_corner_of_curve_sw_e);
}

void test_compass_opposite_south_east() {
	g_assert_true(sem_compass_opposite_of(SEM_SOUTH | SEM_EAST) == (SEM_NORTH | SEM_WEST));
}

void test_compass_opposite_north_west() {
	g_assert_true(sem_compass_opposite_of(SEM_NORTH | SEM_WEST) == (SEM_SOUTH | SEM_EAST));
}

void test_compass_straight() {
	g_assert_true(sem_compass_straight(SEM_SOUTH, SEM_NORTH));
}

void test_compass_not_straight() {
	g_assert_false(sem_compass_straight(SEM_SOUTH | SEM_EAST, SEM_NORTH));
}

void test_compass_corner_of_curve_sw_e() {
	g_assert_true(sem_compass_corner_of_curve(SEM_SOUTH | SEM_WEST, SEM_EAST) == (SEM_SOUTH | SEM_EAST));
}
