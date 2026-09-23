#ifndef VFR_FLIGHT_H
#define VFR_FLIGHT_H

#include <stddef.h>

#define VFR_MAX_AIRCRAFT 128

typedef struct Aircraft {
    char callsign[20];
    char registration[20];
    char aircraft_type[16];
    char category[8];
    char hex[12];
    double latitude;
    double longitude;
    double altitude_ft;
    double speed_kts;
    double heading_deg;
    double distance_nm;
    double bearing_deg;
    int on_ground;
} Aircraft;

int flight_parse_json(const char *json_text,
                      double center_lat,
                      double center_lon,
                      Aircraft *out,
                      int max_aircraft,
                      char *error_text,
                      size_t error_text_size);

double flight_distance_nm(double lat1, double lon1, double lat2, double lon2);
double flight_bearing_deg(double lat1, double lon1, double lat2, double lon2);
void flight_sort_by_distance(Aircraft *aircraft, int count);

#endif
