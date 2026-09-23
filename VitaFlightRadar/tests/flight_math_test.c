#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../src/flight.h"

int main(void) {
    double d0 = flight_distance_nm(32.0853, 34.7818, 32.0853, 34.7818);
    assert(fabs(d0) < 0.0001);

    double north = flight_bearing_deg(32.0, 34.0, 33.0, 34.0);
    assert(north < 0.1 || north > 359.9);

    Aircraft a[3] = {
        {.distance_nm = 30.0},
        {.distance_nm = 5.0},
        {.distance_nm = 12.0}
    };
    flight_sort_by_distance(a, 3);
    assert(a[0].distance_nm == 5.0);
    assert(a[1].distance_nm == 12.0);
    assert(a[2].distance_nm == 30.0);

    puts("flight_math_test: OK");
    return 0;
}
