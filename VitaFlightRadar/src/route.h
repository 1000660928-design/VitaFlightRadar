#ifndef VFR_ROUTE_H
#define VFR_ROUTE_H

#include "flight.h"
#include <stddef.h>
#include <stdint.h>

typedef struct RouteInfo {
    char key[24];
    int found;
    int schedule_found;
    char provider[20];
    char plane_type[64];
    char flight_number[24];
    char airline[64];

    char origin_iata[8];
    char origin_icao[8];
    char origin_city[48];
    char origin_name[80];
    double origin_latitude;
    double origin_longitude;
    int origin_tz_offset;
    int origin_tz_valid;

    char destination_iata[8];
    char destination_icao[8];
    char destination_city[48];
    char destination_name[80];
    double destination_latitude;
    double destination_longitude;
    int destination_tz_offset;
    int destination_tz_valid;

    int64_t scheduled_departure;
    int64_t actual_departure;
    int64_t estimated_departure;
    int64_t scheduled_arrival;
    int64_t actual_arrival;
    int64_t estimated_arrival;
} RouteInfo;

void route_clear(RouteInfo *route);
int route_fetch_for_aircraft(const Aircraft *aircraft,
                             RouteInfo *route,
                             char *error_text,
                             size_t error_text_size);

#endif
