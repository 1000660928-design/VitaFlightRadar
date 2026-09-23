#include "route.h"
#include "net.h"

#include <ctype.h>
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static const char *jstr(json_t *obj, const char *key) {
    if (!json_is_object(obj)) return NULL;
    json_t *v = json_object_get(obj, key);
    return json_is_string(v) ? json_string_value(v) : NULL;
}

static json_t *jobj(json_t *obj, const char *key) {
    if (!json_is_object(obj)) return NULL;
    json_t *v = json_object_get(obj, key);
    return json_is_object(v) ? v : NULL;
}

static json_t *jarr(json_t *obj, const char *key) {
    if (!json_is_object(obj)) return NULL;
    json_t *v = json_object_get(obj, key);
    return json_is_array(v) ? v : NULL;
}

static json_t *path2(json_t *obj, const char *a, const char *b) {
    return jobj(jobj(obj, a), b);
}

static json_t *path3(json_t *obj, const char *a, const char *b, const char *c) {
    return jobj(path2(obj, a, b), c);
}

static const char *str_path2(json_t *obj, const char *a, const char *b) {
    return jstr(jobj(obj, a), b);
}

static const char *str_path3(json_t *obj, const char *a, const char *b, const char *c) {
    return jstr(path2(obj, a, b), c);
}

static int64_t jtimestamp(json_t *obj, const char *key) {
    if (!json_is_object(obj)) return 0;
    json_t *v = json_object_get(obj, key);
    if (json_is_integer(v)) return (int64_t)json_integer_value(v);
    if (json_is_real(v)) return (int64_t)json_real_value(v);
    if (json_is_string(v)) {
        long long n = 0;
        if (sscanf(json_string_value(v), "%lld", &n) == 1) return (int64_t)n;
    }
    return 0;
}

static double jnumber(json_t *obj, const char *key) {
    if (!json_is_object(obj)) return 0.0;
    json_t *v = json_object_get(obj, key);
    if (json_is_number(v)) return json_number_value(v);
    if (json_is_string(v)) {
        double d = 0.0;
        if (sscanf(json_string_value(v), "%lf", &d) == 1) return d;
    }
    return 0.0;
}

static int jint(json_t *obj, const char *key, int *valid) {
    if (valid) *valid = 0;
    if (!json_is_object(obj)) return 0;
    json_t *v = json_object_get(obj, key);
    if (json_is_integer(v)) { if (valid) *valid = 1; return (int)json_integer_value(v); }
    if (json_is_number(v)) { if (valid) *valid = 1; return (int)json_number_value(v); }
    return 0;
}

static int same_trimmed_ci(const char *a, const char *b) {
    char aa[32], bb[32];
    copy_trimmed(aa, sizeof(aa), a);
    copy_trimmed(bb, sizeof(bb), b);
    if (!aa[0] || !bb[0]) return 0;
    size_t na = strlen(aa), nb = strlen(bb);
    if (na != nb) return 0;
    for (size_t i = 0; i < na; i++)
        if (toupper((unsigned char)aa[i]) != toupper((unsigned char)bb[i])) return 0;
    return 1;
}

static int is_unreserved(unsigned char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~';
}

static void url_encode_component(const char *src, char *dst, size_t dst_size) {
    static const char hex[] = "0123456789ABCDEF";
    if (!dst || dst_size == 0) return;
    dst[0] = '\0';
    if (!src) return;
    size_t p = 0;
    for (size_t i = 0; src[i] && p + 1 < dst_size; i++) {
        unsigned char c = (unsigned char)src[i];
        if (is_unreserved(c)) dst[p++] = (char)c;
        else if (p + 3 < dst_size) {
            dst[p++] = '%'; dst[p++] = hex[c >> 4]; dst[p++] = hex[c & 15];
        } else break;
    }
    dst[p] = '\0';
}

void route_clear(RouteInfo *route) {
    if (route) memset(route, 0, sizeof(*route));
}

static void fill_fr24_airport(RouteInfo *route, json_t *airport, int origin) {
    if (!route || !json_is_object(airport)) return;
    json_t *code = jobj(airport, "code");
    json_t *pos = jobj(airport, "position");
    json_t *region = pos ? jobj(pos, "region") : NULL;
    json_t *timezone = jobj(airport, "timezone");

    char *iata = origin ? route->origin_iata : route->destination_iata;
    char *icao = origin ? route->origin_icao : route->destination_icao;
    char *city = origin ? route->origin_city : route->destination_city;
    char *name = origin ? route->origin_name : route->destination_name;
    size_t iata_sz = origin ? sizeof(route->origin_iata) : sizeof(route->destination_iata);
    size_t icao_sz = origin ? sizeof(route->origin_icao) : sizeof(route->destination_icao);
    size_t city_sz = origin ? sizeof(route->origin_city) : sizeof(route->destination_city);
    size_t name_sz = origin ? sizeof(route->origin_name) : sizeof(route->destination_name);

    copy_trimmed(iata, iata_sz, jstr(code, "iata"));
    copy_trimmed(icao, icao_sz, jstr(code, "icao"));
    copy_trimmed(name, name_sz, jstr(airport, "name"));
    const char *city_s = jstr(region, "city");
    if (!city_s) city_s = jstr(airport, "city");
    copy_trimmed(city, city_sz, city_s);

    double plat = jnumber(pos, "latitude");
    double plon = jnumber(pos, "longitude");
    if (origin) { route->origin_latitude = plat; route->origin_longitude = plon; }
    else { route->destination_latitude = plat; route->destination_longitude = plon; }

    int valid = 0;
    int off = jint(timezone, "offset", &valid);
    if (origin) { route->origin_tz_offset = off; route->origin_tz_valid = valid; }
    else { route->destination_tz_offset = off; route->destination_tz_valid = valid; }
}

static int candidate_score(const Aircraft *aircraft, json_t *flight) {
    if (!json_is_object(flight)) return -1000;
    int score = 0;
    json_t *ident = jobj(flight, "identification");
    const char *callsign = jstr(ident, "callsign");
    const char *number = str_path2(ident, "number", "default");
    if (same_trimmed_ci(callsign, aircraft->callsign)) score += 120;
    if (same_trimmed_ci(number, aircraft->callsign)) score += 70;

    json_t *aircraft_obj = jobj(flight, "aircraft");
    const char *reg = str_path2(aircraft_obj, "identification", "registration");
    if (!reg) reg = jstr(aircraft_obj, "registration");
    if (aircraft->registration[0] && same_trimmed_ci(reg, aircraft->registration)) score += 80;

    json_t *time = jobj(flight, "time");
    json_t *real = jobj(time, "real");
    if (jtimestamp(real, "departure") > 0 && jtimestamp(real, "arrival") == 0) score += 35;
    json_t *estimated = jobj(time, "estimated");
    if (jtimestamp(estimated, "arrival") > 0) score += 10;
    return score;
}

static int parse_fr24(const char *body, const Aircraft *aircraft, RouteInfo *route,
                      int require_callsign_match, char *error_text, size_t error_text_size) {
    json_error_t je;
    json_t *root = json_loads(body, 0, &je);
    if (!root) {
        if (error_text && error_text_size) snprintf(error_text, error_text_size, "FR24 JSON: %.60s", je.text);
        return -31;
    }
    json_t *result = jobj(root, "result");
    json_t *response = jobj(result, "response");
    json_t *data = jarr(response, "data");
    if (!json_is_array(data) || json_array_size(data) == 0) {
        json_decref(root);
        return 0;
    }

    json_t *best = NULL;
    int best_score = -10000;
    size_t i;
    json_t *f;
    json_array_foreach(data, i, f) {
        int s = candidate_score(aircraft, f);
        if (!best || s > best_score) { best = f; best_score = s; }
    }
    if (!best || (require_callsign_match && best_score < 100)) { json_decref(root); return 0; }

    json_t *ident = jobj(best, "identification");
    copy_trimmed(route->flight_number, sizeof(route->flight_number), str_path2(ident, "number", "default"));
    if (!route->flight_number[0]) copy_trimmed(route->flight_number, sizeof(route->flight_number), jstr(ident, "callsign"));

    json_t *airline = jobj(best, "airline");
    copy_trimmed(route->airline, sizeof(route->airline), jstr(airline, "name"));

    json_t *aircraft_obj = jobj(best, "aircraft");
    json_t *model = jobj(aircraft_obj, "model");
    const char *ptype = jstr(model, "text");
    if (!ptype) ptype = jstr(model, "code");
    if (!ptype) ptype = str_path2(aircraft_obj, "identification", "model");
    if (ptype) copy_trimmed(route->plane_type, sizeof(route->plane_type), ptype);

    json_t *airport = jobj(best, "airport");
    json_t *origin = jobj(airport, "origin");
    json_t *dest = jobj(airport, "real");
    if (!dest) dest = jobj(airport, "destination");
    if (origin) fill_fr24_airport(route, origin, 1);
    if (dest) fill_fr24_airport(route, dest, 0);

    json_t *time = jobj(best, "time");
    json_t *scheduled = jobj(time, "scheduled");
    json_t *actual = jobj(time, "real");
    json_t *estimated = jobj(time, "estimated");
    route->scheduled_departure = jtimestamp(scheduled, "departure");
    route->scheduled_arrival = jtimestamp(scheduled, "arrival");
    route->actual_departure = jtimestamp(actual, "departure");
    route->actual_arrival = jtimestamp(actual, "arrival");
    route->estimated_departure = jtimestamp(estimated, "departure");
    route->estimated_arrival = jtimestamp(estimated, "arrival");

    if ((route->origin_iata[0] || route->origin_icao[0]) &&
        (route->destination_iata[0] || route->destination_icao[0])) {
        route->found = 1;
        copy_trimmed(route->provider, sizeof(route->provider), "fr24-web");
    }
    if (route->scheduled_departure || route->scheduled_arrival || route->actual_departure ||
        route->estimated_departure || route->actual_arrival || route->estimated_arrival)
        route->schedule_found = 1;

    json_decref(root);
    return (route->found || route->schedule_found) ? 1 : 0;
}

static int fr24_request(const Aircraft *aircraft, const char *value, const char *fetch_by,
                        int require_callsign_match, RouteInfo *route,
                        char *error_text, size_t error_text_size) {
    char query[96];
    char url[512];
    url_encode_component(value, query, sizeof(query));
    snprintf(url, sizeof(url),
             "https://api.flightradar24.com/common/v1/flight/list.json?query=%s&fetchBy=%s&page=1&limit=25&token=",
             query, fetch_by);
    char *body = NULL;
    size_t body_size = 0;
    long status = 0;
    int rc = vfr_http_get(url, &body, &body_size, &status, error_text, error_text_size);
    if (rc < 0) return rc;
    int result = 0;
    if (status == 200 && body && body_size > 8)
        result = parse_fr24(body, aircraft, route, require_callsign_match, error_text, error_text_size);
    else if (status == 404) result = 0;
    else result = -32;
    vfr_http_free(body);
    return result;
}

static int fr24_get(const Aircraft *aircraft, RouteInfo *route,
                    char *error_text, size_t error_text_size) {
    int rc = 0;
    if (aircraft->registration[0]) {
        rc = fr24_request(aircraft, aircraft->registration, "reg", 1, route, error_text, error_text_size);
        if (rc > 0 && (route->found || route->schedule_found)) return rc;
        route_clear(route);
        copy_trimmed(route->key, sizeof(route->key), aircraft->callsign);
        if (aircraft->aircraft_type[0]) copy_trimmed(route->plane_type, sizeof(route->plane_type), aircraft->aircraft_type);
    }
    return fr24_request(aircraft, aircraft->callsign, "flight", 0, route, error_text, error_text_size);
}

static double jnum_alias(json_t *obj, const char *a, const char *b) {
    if (!json_is_object(obj)) return 0.0;
    json_t *v = json_object_get(obj, a);
    if (!json_is_number(v) && b) v = json_object_get(obj, b);
    if (json_is_number(v)) return json_number_value(v);
    if (json_is_string(v)) {
        double d = 0.0;
        if (sscanf(json_string_value(v), "%lf", &d) == 1) return d;
    }
    return 0.0;
}

static void fill_airport_adsbdb(RouteInfo *route, json_t *airport, int origin) {
    if (!route || !json_is_object(airport)) return;
    if (origin) {
        copy_trimmed(route->origin_iata, sizeof(route->origin_iata), jstr(airport, "iata_code"));
        copy_trimmed(route->origin_icao, sizeof(route->origin_icao), jstr(airport, "icao_code"));
        copy_trimmed(route->origin_city, sizeof(route->origin_city), jstr(airport, "municipality"));
        copy_trimmed(route->origin_name, sizeof(route->origin_name), jstr(airport, "name"));
        route->origin_latitude = jnum_alias(airport, "latitude", NULL);
        route->origin_longitude = jnum_alias(airport, "longitude", NULL);
    } else {
        copy_trimmed(route->destination_iata, sizeof(route->destination_iata), jstr(airport, "iata_code"));
        copy_trimmed(route->destination_icao, sizeof(route->destination_icao), jstr(airport, "icao_code"));
        copy_trimmed(route->destination_city, sizeof(route->destination_city), jstr(airport, "municipality"));
        copy_trimmed(route->destination_name, sizeof(route->destination_name), jstr(airport, "name"));
        route->destination_latitude = jnum_alias(airport, "latitude", NULL);
        route->destination_longitude = jnum_alias(airport, "longitude", NULL);
    }
}

static int parse_adsbdb(const char *body, const Aircraft *aircraft, RouteInfo *route,
                        char *error_text, size_t error_text_size) {
    json_error_t je;
    json_t *root = json_loads(body, 0, &je);
    if (!root) return -40;
    json_t *response = jobj(root, "response");
    if (!response) { json_decref(root); return 0; }
    json_t *fr = jobj(response, "flightroute");
    json_t *air = jobj(response, "aircraft");
    if (air) {
        const char *type = jstr(air, "type");
        if (type && *type) copy_trimmed(route->plane_type, sizeof(route->plane_type), type);
    }
    if (!route->plane_type[0] && aircraft->aircraft_type[0])
        copy_trimmed(route->plane_type, sizeof(route->plane_type), aircraft->aircraft_type);
    if (fr) {
        json_t *origin = jobj(fr, "origin");
        json_t *dest = jobj(fr, "destination");
        if (origin && dest) {
            fill_airport_adsbdb(route, origin, 1);
            fill_airport_adsbdb(route, dest, 0);
            copy_trimmed(route->flight_number, sizeof(route->flight_number), jstr(fr, "callsign_iata"));
            if (!route->flight_number[0]) copy_trimmed(route->flight_number, sizeof(route->flight_number), jstr(fr, "callsign"));
            json_t *al = jobj(fr, "airline");
            if (al) copy_trimmed(route->airline, sizeof(route->airline), jstr(al, "name"));
            if ((route->origin_iata[0] || route->origin_icao[0]) &&
                (route->destination_iata[0] || route->destination_icao[0])) {
                route->found = 1;
                copy_trimmed(route->provider, sizeof(route->provider), "adsbdb");
            }
        }
    }
    json_decref(root);
    return route->found ? 1 : 0;
}

static int adsbdb_get(const Aircraft *aircraft, RouteInfo *route,
                      char *error_text, size_t error_text_size) {
    char cs[64], url[384];
    url_encode_component(aircraft->callsign, cs, sizeof(cs));
    snprintf(url, sizeof(url), "https://api.adsbdb.com/v0/callsign/%s", cs);
    char *body = NULL; size_t body_size = 0; long status = 0;
    int rc = vfr_http_get(url, &body, &body_size, &status, error_text, error_text_size);
    if (rc < 0) return rc;
    int result = 0;
    if (status == 200) result = parse_adsbdb(body, aircraft, route, error_text, error_text_size);
    else if (status == 404) result = 0;
    else result = -41;
    vfr_http_free(body);
    return result;
}

int route_fetch_for_aircraft(const Aircraft *aircraft,
                             RouteInfo *route,
                             char *error_text,
                             size_t error_text_size) {
    if (error_text && error_text_size) error_text[0] = '\0';
    if (!aircraft || !route) return -1;
    route_clear(route);
    copy_trimmed(route->key, sizeof(route->key), aircraft->callsign);
    if (aircraft->aircraft_type[0]) copy_trimmed(route->plane_type, sizeof(route->plane_type), aircraft->aircraft_type);
    if (!aircraft->callsign[0] || !strcmp(aircraft->callsign, "UNKNOWN")) return 0;

    /* First choice: FR24 web flight-list data, because it can contain the timetable fields the UI shows. */
    int rc = fr24_get(aircraft, route, error_text, error_text_size);
    if (rc > 0 && (route->found || route->schedule_found)) return rc;

    /* Route fallback: ADSBdb public callsign database. */
    route_clear(route);
    copy_trimmed(route->key, sizeof(route->key), aircraft->callsign);
    if (aircraft->aircraft_type[0]) copy_trimmed(route->plane_type, sizeof(route->plane_type), aircraft->aircraft_type);
    rc = adsbdb_get(aircraft, route, error_text, error_text_size);
    return rc;
}
