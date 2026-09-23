#include "flight.h"

#include <jansson.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double to_rad(double v) { return v * M_PI / 180.0; }
static double to_deg(double v) { return v * 180.0 / M_PI; }

static void copy_trimmed(char *dst, size_t dst_size, const char *src) {
    if (!dst || dst_size == 0) return;
    dst[0] = '\0';
    if (!src) return;

    while (*src == ' ' || *src == '\t') src++;
    size_t n = strlen(src);
    while (n > 0 && (src[n - 1] == ' ' || src[n - 1] == '\t' || src[n - 1] == '\r' || src[n - 1] == '\n')) n--;
    if (n >= dst_size) n = dst_size - 1;
    memcpy(dst, src, n);
    dst[n] = '\0';
}

static const char *json_string_alias(json_t *obj, const char *a, const char *b, const char *c) {
    const char *keys[3] = {a, b, c};
    for (int i = 0; i < 3; i++) {
        if (!keys[i]) continue;
        json_t *v = json_object_get(obj, keys[i]);
        if (json_is_string(v)) return json_string_value(v);
    }
    return NULL;
}

static double json_number_alias(json_t *obj, const char *a, const char *b, double fallback) {
    json_t *v = json_object_get(obj, a);
    if (!json_is_number(v) && b) v = json_object_get(obj, b);
    if (json_is_number(v)) return json_number_value(v);
    return fallback;
}

double flight_distance_nm(double lat1, double lon1, double lat2, double lon2) {
    const double earth_radius_nm = 3440.065;
    double p1 = to_rad(lat1);
    double p2 = to_rad(lat2);
    double dp = to_rad(lat2 - lat1);
    double dl = to_rad(lon2 - lon1);
    double a = sin(dp / 2.0) * sin(dp / 2.0) +
               cos(p1) * cos(p2) * sin(dl / 2.0) * sin(dl / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    return earth_radius_nm * c;
}

double flight_bearing_deg(double lat1, double lon1, double lat2, double lon2) {
    double p1 = to_rad(lat1);
    double p2 = to_rad(lat2);
    double dl = to_rad(lon2 - lon1);
    double y = sin(dl) * cos(p2);
    double x = cos(p1) * sin(p2) - sin(p1) * cos(p2) * cos(dl);
    double b = fmod(to_deg(atan2(y, x)) + 360.0, 360.0);
    return b;
}

void flight_sort_by_distance(Aircraft *aircraft, int count) {
    if (!aircraft || count <= 1) return;
    for (int i = 1; i < count; i++) {
        Aircraft key = aircraft[i];
        int j = i - 1;
        while (j >= 0 && aircraft[j].distance_nm > key.distance_nm) {
            aircraft[j + 1] = aircraft[j];
            j--;
        }
        aircraft[j + 1] = key;
    }
}

int flight_parse_json(const char *json_text,
                      double center_lat,
                      double center_lon,
                      Aircraft *out,
                      int max_aircraft,
                      char *error_text,
                      size_t error_text_size) {
    if (error_text && error_text_size) error_text[0] = '\0';
    if (!json_text || !out || max_aircraft <= 0) return -1;

    json_error_t jerr;
    json_t *root = json_loads(json_text, 0, &jerr);
    if (!root) {
        if (error_text && error_text_size)
            snprintf(error_text, error_text_size, "JSON error: %.80s", jerr.text);
        return -2;
    }

    json_t *arr = json_object_get(root, "ac");
    if (!json_is_array(arr)) arr = json_object_get(root, "aircraft");
    if (!json_is_array(arr)) {
        if (error_text && error_text_size)
            snprintf(error_text, error_text_size, "API response did not contain an aircraft array");
        json_decref(root);
        return -3;
    }

    int count = 0;
    size_t index;
    json_t *obj;
    json_array_foreach(arr, index, obj) {
        if (count >= max_aircraft) break;
        if (!json_is_object(obj)) continue;

        json_t *latv = json_object_get(obj, "lat");
        json_t *lonv = json_object_get(obj, "lon");
        if (!json_is_number(latv) || !json_is_number(lonv)) continue;

        Aircraft a;
        memset(&a, 0, sizeof(a));
        a.latitude = json_number_value(latv);
        a.longitude = json_number_value(lonv);
        a.speed_kts = json_number_alias(obj, "gs", "ground_speed", 0.0);
        a.heading_deg = json_number_alias(obj, "track", "heading", 0.0);

        json_t *altv = json_object_get(obj, "alt_baro");
        if (!altv) altv = json_object_get(obj, "alt_geom");
        if (json_is_string(altv) && strcmp(json_string_value(altv), "ground") == 0) {
            a.on_ground = 1;
            a.altitude_ft = 0.0;
        } else if (json_is_number(altv)) {
            a.altitude_ft = json_number_value(altv);
        }

        copy_trimmed(a.callsign, sizeof(a.callsign), json_string_alias(obj, "flight", "callsign", NULL));
        copy_trimmed(a.registration, sizeof(a.registration), json_string_alias(obj, "r", "registration", "reg"));
        copy_trimmed(a.aircraft_type, sizeof(a.aircraft_type), json_string_alias(obj, "t", "type", NULL));
        copy_trimmed(a.category, sizeof(a.category), json_string_alias(obj, "category", NULL, NULL));
        copy_trimmed(a.hex, sizeof(a.hex), json_string_alias(obj, "hex", "icao", NULL));

        if (a.callsign[0] == '\0') {
            if (a.registration[0] != '\0') copy_trimmed(a.callsign, sizeof(a.callsign), a.registration);
            else if (a.hex[0] != '\0') copy_trimmed(a.callsign, sizeof(a.callsign), a.hex);
            else copy_trimmed(a.callsign, sizeof(a.callsign), "UNKNOWN");
        }

        a.distance_nm = flight_distance_nm(center_lat, center_lon, a.latitude, a.longitude);
        a.bearing_deg = flight_bearing_deg(center_lat, center_lon, a.latitude, a.longitude);
        out[count++] = a;
    }

    json_decref(root);
    flight_sort_by_distance(out, count);
    return count;
}
