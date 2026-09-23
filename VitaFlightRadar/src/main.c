#include "app_config.h"
#include "flight.h"
#include "map.h"
#include "net.h"
#include "route.h"

#include <ctype.h>
#include <math.h>
#include <psp2/apputil.h>
#include <psp2/common_dialog.h>
#include <psp2/ctrl.h>
#include <psp2/ime_dialog.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/rtc.h>
#include <psp2/sysmodule.h>
#include <psp2/touch.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vita2d.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

unsigned int _newlib_heap_size_user = 64 * 1024 * 1024;

#define SCREEN_W 960
#define SCREEN_H 544
#define TOP_H 52
#define MAP_X 0.0f
#define MAP_Y 52.0f
#define MAP_W 650.0f
#define MAP_H 492.0f
#define PANEL_X 650.0f
#define PANEL_W 310.0f

static Aircraft g_aircraft[VFR_MAX_AIRCRAFT];
static int g_aircraft_count = 0;
static int g_selected = -1;
static SceUInt64 g_last_refresh_us = 0;
static SceUInt64 g_last_route_refresh_us = 0;
static int g_auto_refresh = 1;
static int g_map_zoom = 13;
static RouteInfo g_route;
static MapState g_map;
static char g_map_status[64] = "SATELLITE WAIT";
static char g_status[96] = "Starting";

static unsigned int COL_BG;
static unsigned int COL_PANEL;
static unsigned int COL_TEXT;
static unsigned int COL_DIM;
static unsigned int COL_ACCENT;
static unsigned int COL_SELECTED;
static unsigned int COL_CARD;

static float deg_to_rad(float v) { return v * (float)M_PI / 180.0f; }

static void btext(vita2d_pgf *font, int x, int y, float scale, unsigned int color, const char *s) {
    const char *v = s ? s : "";
    /* v1.8: very small four-pass text looked muddy on the Vita LCD. Keep a
       readable minimum size and use a lighter two-pass weight. */
    if (scale < 0.64f) scale = 0.64f;
    vita2d_pgf_draw_text(font, x, y, color, scale, v);
    vita2d_pgf_draw_text(font, x + 1, y, color, scale, v);
}

static void btextf(vita2d_pgf *font, int x, int y, float scale, unsigned int color, const char *fmt, ...) {
    char buf[192];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    btext(font, x, y, scale, color, buf);
}

static int same_ci(const char *a, const char *b) {
    if (!a || !b || !*a || !*b) return 0;
    while (*a && *b) {
        if (toupper((unsigned char)*a) != toupper((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

static int known_non_plane(const Aircraft *a) {
    if (!a) return 1;
    if (!strcmp(a->category, "A7")) return 1; /* rotorcraft */
    if (a->category[0] == 'B' && a->category[1] >= '1' && a->category[1] <= '7') return 1;
    if (a->category[0] == 'C') return 1;
    return 0;
}

static int request_radius_nm(int range_km) {
    int nm = (int)ceil((double)range_km / 1.852);
    if (nm < 1) nm = 1;
    if (nm > 250) nm = 250;
    return nm;
}

static int in_selected_radius(const Aircraft *a, const AppConfig *cfg) {
    if (!a || !cfg) return 0;
    return a->distance_nm * 1.852 <= (double)cfg->range_km + 0.001;
}

static void preserve_selection(char *hex, size_t hex_size, char *callsign, size_t callsign_size) {
    if (hex_size) hex[0] = '\0';
    if (callsign_size) callsign[0] = '\0';
    if (g_selected >= 0 && g_selected < g_aircraft_count) {
        snprintf(hex, hex_size, "%s", g_aircraft[g_selected].hex);
        snprintf(callsign, callsign_size, "%s", g_aircraft[g_selected].callsign);
    }
}

static void restore_selection(const char *hex, const char *callsign) {
    if (g_aircraft_count <= 0) { g_selected = -1; return; }
    if (hex && *hex) {
        for (int i = 0; i < g_aircraft_count; i++)
            if (!strcmp(g_aircraft[i].hex, hex)) { g_selected = i; return; }
    }
    if (callsign && *callsign) {
        for (int i = 0; i < g_aircraft_count; i++)
            if (!strcmp(g_aircraft[i].callsign, callsign)) { g_selected = i; return; }
    }
    g_selected = 0;
}

static int refresh_flights(const AppConfig *cfg) {
    char url[256];
    snprintf(url, sizeof(url),
             "https://opendata.adsb.fi/api/v3/lat/%.6f/lon/%.6f/dist/%d",
             cfg->latitude, cfg->longitude, request_radius_nm(cfg->range_km));

    char keep_hex[12], keep_callsign[20];
    preserve_selection(keep_hex, sizeof(keep_hex), keep_callsign, sizeof(keep_callsign));

    char *body = NULL;
    size_t body_size = 0;
    long http_status = 0;
    char error[128];
    int rc = vfr_http_get(url, &body, &body_size, &http_status, error, sizeof(error));
    if (rc < 0) {
        snprintf(g_status, sizeof(g_status), "AIRCRAFT NET ERROR");
        return rc;
    }
    if (http_status != 200) {
        snprintf(g_status, sizeof(g_status), "AIRCRAFT HTTP %ld", http_status);
        vfr_http_free(body);
        return -10;
    }

    Aircraft temp[VFR_MAX_AIRCRAFT];
    int parsed = flight_parse_json(body, cfg->latitude, cfg->longitude,
                                   temp, VFR_MAX_AIRCRAFT, error, sizeof(error));
    vfr_http_free(body);
    if (parsed < 0) {
        snprintf(g_status, sizeof(g_status), "AIRCRAFT DATA ERROR");
        return parsed;
    }

    int count = 0;
    for (int i = 0; i < parsed && count < VFR_MAX_AIRCRAFT; i++) {
        if (!in_selected_radius(&temp[i], cfg)) continue;
        if (known_non_plane(&temp[i])) continue;
        g_aircraft[count++] = temp[i];
    }
    g_aircraft_count = count;
    restore_selection(keep_hex, keep_callsign);
    g_last_refresh_us = sceKernelGetProcessTimeWide();
    snprintf(g_status, sizeof(g_status), "%d PLANES IN %d KM", count, cfg->range_km);
    return count;
}

static int ensure_network_and_refresh(const AppConfig *cfg, int *net_ok) {
    if (*net_ok < 0) {
        char error[128];
        *net_ok = vfr_net_init(error, sizeof(error));
        if (*net_ok < 0) {
            snprintf(g_status, sizeof(g_status), "NETWORK INIT ERROR");
            return *net_ok;
        }
    }
    return refresh_flights(cfg);
}

static void refresh_route_for_selection(void) {
    route_clear(&g_route);
    if (g_selected < 0 || g_selected >= g_aircraft_count) return;
    const Aircraft *a = &g_aircraft[g_selected];
    if (!a->callsign[0] || !strcmp(a->callsign, "UNKNOWN")) return;
    char err[128];
    route_fetch_for_aircraft(a, &g_route, err, sizeof(err));
    g_last_route_refresh_us = sceKernelGetProcessTimeWide();
}

static void apply_packaged_location(AppConfig *cfg) {
    FILE *fp = fopen("app0:default_location.txt", "r");
    if (!fp) return;
    double lat = 0.0, lon = 0.0;
    int got = fscanf(fp, "%lf,%lf", &lat, &lon);
    fclose(fp);
    if (got == 2 && lat >= -90.0 && lat <= 90.0 && lon >= -180.0 && lon <= 180.0) {
        cfg->latitude = lat;
        cfg->longitude = lon;
    }
}

static void aircraft_screen_pos(const Aircraft *a, float *x, float *y) {
    map_geo_to_screen(&g_map, a->latitude, a->longitude, x, y);
}

static void thick_line(float x0, float y0, float x1, float y1, unsigned int color) {
    vita2d_draw_line(x0, y0, x1, y1, color);
    vita2d_draw_line(x0 + 1.0f, y0, x1 + 1.0f, y1, color);
    vita2d_draw_line(x0, y0 + 1.0f, x1, y1 + 1.0f, color);
}

static void draw_plane_icon(const Aircraft *a, int selected) {
    float x, y;
    aircraft_screen_pos(a, &x, &y);
    if (x < MAP_X - 14 || x > MAP_X + MAP_W + 14 || y < MAP_Y - 14 || y > MAP_Y + MAP_H + 14) return;

    float h = deg_to_rad((float)a->heading_deg);
    float fwdx = sinf(h), fwdy = -cosf(h);
    float rightx = cosf(h), righty = sinf(h);
    float s = selected ? 1.35f : 1.12f;
    unsigned int color = selected ? COL_SELECTED : COL_ACCENT;

    float nose_x = x + fwdx * 11.0f * s;
    float nose_y = y + fwdy * 11.0f * s;
    float tail_x = x - fwdx * 9.0f * s;
    float tail_y = y - fwdy * 9.0f * s;
    float wing_cx = x - fwdx * 0.5f * s;
    float wing_cy = y - fwdy * 0.5f * s;
    float left_wx = wing_cx + rightx * 8.0f * s;
    float left_wy = wing_cy + righty * 8.0f * s;
    float right_wx = wing_cx - rightx * 8.0f * s;
    float right_wy = wing_cy - righty * 8.0f * s;
    float stab_cx = x - fwdx * 6.0f * s;
    float stab_cy = y - fwdy * 6.0f * s;
    float left_tx = stab_cx + rightx * 4.0f * s;
    float left_ty = stab_cy + righty * 4.0f * s;
    float right_tx = stab_cx - rightx * 4.0f * s;
    float right_ty = stab_cy - righty * 4.0f * s;

    if (selected) vita2d_draw_fill_circle(x, y, 13.0f, RGBA8(10, 20, 25, 175));
    thick_line(nose_x, nose_y, tail_x, tail_y, color);
    thick_line(left_wx, left_wy, right_wx, right_wy, color);
    thick_line(left_tx, left_ty, right_tx, right_ty, color);
    vita2d_draw_fill_circle(x, y, 2.5f, color);
}

static void draw_clock(vita2d_pgf *font) {
    SceDateTime dt;
    if (sceRtcGetCurrentClockLocalTime(&dt) >= 0)
        btextf(font, 765, 35, 0.72f, COL_TEXT, "%02d/%02d/%02d  %02d:%02d",
               dt.day, dt.month, dt.year % 100, dt.hour, dt.minute);
}

static void draw_compass(vita2d_pgf *font) {
    const float cx = 613.0f, cy = 91.0f;
    vita2d_draw_fill_circle(cx, cy, 28.0f, RGBA8(12, 20, 26, 205));
    vita2d_draw_fill_circle(cx, cy, 25.0f, RGBA8(24, 34, 40, 220));
    thick_line(cx, cy + 13, cx, cy - 16, COL_SELECTED);
    thick_line(cx, cy - 16, cx - 5, cy - 7, COL_SELECTED);
    thick_line(cx, cy - 16, cx + 5, cy - 7, COL_SELECTED);
    btext(font, 606, 72, 0.58f, COL_TEXT, "N");
}

static void draw_map(vita2d_pgf *font, const AppConfig *cfg) {
    vita2d_draw_rectangle(MAP_X, MAP_Y, MAP_W, MAP_H, RGBA8(20, 28, 31, 255));
    map_draw(&g_map);

    /* Slight darkening keeps plane icons and labels readable over bright imagery. */
    vita2d_draw_rectangle(MAP_X, MAP_Y, MAP_W, MAP_H, RGBA8(0, 0, 0, 28));

    /* The screen center is the live tracking point. While the user drags the
       map, geography moves under this fixed marker; releasing the drag commits
       the new center and refreshes aircraft around it. */
    float ux = MAP_X + MAP_W * 0.5f;
    float uy = MAP_Y + MAP_H * 0.5f;
    vita2d_draw_fill_circle(ux, uy, 7.0f, RGBA8(255, 255, 255, 235));
    vita2d_draw_fill_circle(ux, uy, 4.0f, COL_SELECTED);
    btext(font, (int)ux + 10, (int)uy + 6, 0.68f, COL_TEXT, "TRACKING CENTER");

    for (int i = 0; i < g_aircraft_count; i++) draw_plane_icon(&g_aircraft[i], i == g_selected);
    draw_compass(font);

    vita2d_draw_rectangle(12, 64, 258, 31, RGBA8(8, 16, 20, 205));
    btext(font, 22, 86, 0.66f, COL_TEXT, "SATELLITE  •  STREAMING TILE MAP");

    vita2d_draw_rectangle(12, 493, 626, 43, RGBA8(8, 16, 20, 220));
    btextf(font, 22, 514, 0.66f, COL_TEXT, "VIEW ~%d KM  •  DRAG MAP  •  PINCH ZOOM", cfg->range_km);
    btextf(font, 22, 533, 0.64f, COL_DIM, "HOLD TO RECENTER  •  %s", g_map_status);
    btextf(font, 588, 514, 0.66f, g_map.loaded_count > 0 ? COL_ACCENT : RGBA8(255, 145, 115, 255), "Z%d", g_map.zoom);
}

static void format_epoch(char *out, size_t out_size, int64_t epoch, int tz_offset, int tz_valid) {
    if (!out || out_size == 0) return;
    if (epoch <= 0) { snprintf(out, out_size, "--:--"); return; }
    long long local = (long long)epoch + (tz_valid ? (long long)tz_offset : 0LL);
    long long sec = local % 86400LL;
    if (sec < 0) sec += 86400LL;
    snprintf(out, out_size, "%02lld:%02lld", sec / 3600LL, (sec % 3600LL) / 60LL);
}

static int route_progress_percent(const Aircraft *a, const RouteInfo *r) {
    if (!a || !r || !r->found || r->origin_latitude == 0.0 || r->destination_latitude == 0.0) return -1;
    double from_nm = flight_distance_nm(r->origin_latitude, r->origin_longitude, a->latitude, a->longitude);
    double to_nm = flight_distance_nm(a->latitude, a->longitude, r->destination_latitude, r->destination_longitude);
    if (from_nm + to_nm < 5.0) return -1;
    int pct = (int)(from_nm / (from_nm + to_nm) * 100.0 + 0.5);
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    return pct;
}

static void time_card(vita2d_pgf *font, int x, int y, const char *label, const char *value) {
    vita2d_draw_rectangle((float)x, (float)y, 137, 53, COL_CARD);
    btext(font, x + 9, y + 19, 0.58f, COL_DIM, label);
    btext(font, x + 9, y + 45, 0.80f, COL_TEXT, value);
}

static void metric_card(vita2d_pgf *font, int x, int y, const char *label, const char *value) {
    vita2d_draw_rectangle((float)x, (float)y, 137, 46, COL_CARD);
    btext(font, x + 9, y + 17, 0.58f, COL_DIM, label);
    btext(font, x + 9, y + 40, 0.73f, COL_TEXT, value);
}

static void draw_panel(vita2d_pgf *font) {
    vita2d_draw_rectangle(PANEL_X, TOP_H, PANEL_W, SCREEN_H - TOP_H, COL_PANEL);
    btext(font, 666, 79, 0.66f, COL_DIM, "SELECTED FLIGHT");

    if (g_selected < 0 || g_selected >= g_aircraft_count) {
        btext(font, 670, 130, 0.78f, COL_TEXT, "NO PLANE SELECTED");
        btext(font, 670, 165, 0.64f, COL_DIM, "UP/DOWN TO SELECT");
        btext(font, 670, 505, 0.64f, COL_TEXT, "DRAG MAP  •  PINCH ZOOM");
        btext(font, 670, 532, 0.64f, COL_TEXT, "HOLD RECENTER  •  TAP PLANE");
        return;
    }

    const Aircraft *a = &g_aircraft[g_selected];
    btextf(font, 666, 116, 1.08f, COL_SELECTED, "%s", a->callsign);

    const char *ptype = g_route.plane_type[0] ? g_route.plane_type :
                        (a->aircraft_type[0] ? a->aircraft_type : "UNKNOWN");
    if (g_route.airline[0])
        btextf(font, 666, 145, 0.62f, COL_TEXT, "%.15s  •  %.17s", ptype, g_route.airline);
    else
        btextf(font, 666, 145, 0.62f, COL_TEXT, "PLANE  %.26s", ptype);

    int private_no_route = !g_route.found && a->registration[0] && same_ci(a->callsign, a->registration);
    const char *from_code = "---";
    const char *to_code = "---";
    const char *from_city = private_no_route ? "PRIVATE FLIGHT" : "ROUTE UNAVAILABLE";
    const char *to_city = private_no_route ? "NO PUBLIC SCHEDULE" : "ROUTE UNAVAILABLE";
    if (g_route.found) {
        from_code = g_route.origin_iata[0] ? g_route.origin_iata : g_route.origin_icao;
        to_code = g_route.destination_iata[0] ? g_route.destination_iata : g_route.destination_icao;
        from_city = g_route.origin_city[0] ? g_route.origin_city : g_route.origin_name;
        to_city = g_route.destination_city[0] ? g_route.destination_city : g_route.destination_name;
    }

    vita2d_draw_rectangle(664, 158, 282, 78, COL_CARD);
    btext(font, 675, 179, 0.58f, COL_DIM, "FROM");
    btext(font, 842, 179, 0.58f, COL_DIM, "TO");
    btextf(font, 675, 209, 0.94f, COL_TEXT, "%s", from_code);
    btext(font, 797, 209, 0.90f, COL_ACCENT, ">");
    btextf(font, 842, 209, 0.94f, COL_TEXT, "%s", to_code);
    btextf(font, 675, 230, 0.56f, COL_DIM, "%.17s", from_city);
    btextf(font, 813, 230, 0.56f, COL_DIM, "%.17s", to_city);

    char sched_dep[12], dep_best[12], sched_arr[12], arr_best[12];
    format_epoch(sched_dep, sizeof(sched_dep), g_route.scheduled_departure, g_route.origin_tz_offset, g_route.origin_tz_valid);
    format_epoch(dep_best, sizeof(dep_best), g_route.actual_departure ? g_route.actual_departure : g_route.estimated_departure,
                 g_route.origin_tz_offset, g_route.origin_tz_valid);
    format_epoch(sched_arr, sizeof(sched_arr), g_route.scheduled_arrival, g_route.destination_tz_offset, g_route.destination_tz_valid);
    format_epoch(arr_best, sizeof(arr_best), g_route.estimated_arrival ? g_route.estimated_arrival : g_route.actual_arrival,
                 g_route.destination_tz_offset, g_route.destination_tz_valid);

    time_card(font, 664, 244, "DEP SCHED", sched_dep);
    time_card(font, 807, 244, g_route.actual_departure ? "DEP ACTUAL" : "DEP EST", dep_best);
    time_card(font, 664, 303, "ARR SCHED", sched_arr);
    time_card(font, 807, 303, g_route.actual_arrival ? "ARR ACTUAL" : "ARR EST", arr_best);

    int pct = route_progress_percent(a, &g_route);
    vita2d_draw_rectangle(664, 365, 280, 9, RGBA8(62, 75, 83, 255));
    if (pct >= 0) vita2d_draw_rectangle(664, 365, 2.8f * (float)pct, 9, COL_ACCENT);

    char alt[24], spd[24], dist[24], hdg[24];
    snprintf(alt, sizeof(alt), "%.0f ft", a->altitude_ft);
    snprintf(spd, sizeof(spd), "%.0f kt", a->speed_kts);
    snprintf(dist, sizeof(dist), "%.1f km", a->distance_nm * 1.852);
    snprintf(hdg, sizeof(hdg), "%.0f deg", a->heading_deg);
    metric_card(font, 664, 384, "ALTITUDE", alt);
    metric_card(font, 807, 384, "SPEED", spd);
    metric_card(font, 664, 436, "DISTANCE", dist);
    metric_card(font, 807, 436, "HEADING", hdg);

    btext(font, 666, 505, 0.64f, COL_TEXT, "DRAG MAP  •  PINCH ZOOM");
    btext(font, 666, 532, 0.64f, COL_TEXT, "TRIANGLE COORDS  •  TAP PLANE");
}

static void draw_ui(vita2d_pgf *font, const AppConfig *cfg) {
    vita2d_start_drawing();
    vita2d_clear_screen();
    vita2d_draw_rectangle(0, 0, SCREEN_W, TOP_H, RGBA8(20, 31, 38, 255));
    btext(font, 18, 37, 0.92f, COL_TEXT, "VITA FLIGHT MAP");
    btextf(font, 325, 35, 0.72f, g_auto_refresh ? COL_ACCENT : COL_DIM,
           "%s  VIEW ~%d KM  Z%d", g_auto_refresh ? "AUTO" : "MANUAL", cfg->range_km, g_map.zoom);
    draw_clock(font);
    draw_map(font, cfg);
    draw_panel(font);
    vita2d_end_drawing();
    vita2d_swap_buffers();
}

static void draw_loading(vita2d_pgf *font, const char *message) {
    vita2d_start_drawing();
    vita2d_clear_screen();
    btext(font, 70, 225, 1.10f, COL_TEXT, "VITA FLIGHT MAP");
    btext(font, 70, 285, 0.78f, COL_ACCENT, message);
    vita2d_end_drawing();
    vita2d_swap_buffers();
}

static void ascii_to_utf16(const char *src, SceWChar16 *dst, size_t cap) {
    if (!dst || cap == 0) return;
    size_t i = 0;
    if (src) {
        for (; src[i] && i + 1 < cap; i++) dst[i] = (SceWChar16)(unsigned char)src[i];
    }
    dst[i] = 0;
}

static void utf16_to_ascii(const SceWChar16 *src, char *dst, size_t cap) {
    if (!dst || cap == 0) return;
    size_t i = 0;
    if (src) {
        for (; src[i] && i + 1 < cap; i++) {
            unsigned int c = (unsigned int)src[i];
            dst[i] = (c >= 32 && c <= 126) ? (char)c : ' ';
        }
    }
    dst[i] = '\0';
}

static int parse_coordinates(const char *text, double *lat, double *lon) {
    if (!text || !lat || !lon) return 0;
    char *end1 = NULL;
    double a = strtod(text, &end1);
    if (end1 == text) return 0;
    while (*end1 == ' ' || *end1 == '\t') end1++;
    if (*end1 == ',' || *end1 == ';') end1++;
    while (*end1 == ' ' || *end1 == '\t') end1++;
    char *end2 = NULL;
    double b = strtod(end1, &end2);
    if (end2 == end1) return 0;
    while (*end2 == ' ' || *end2 == '\t') end2++;
    if (*end2 != '\0') return 0;
    if (a < -90.0 || a > 90.0 || b < -180.0 || b > 180.0) return 0;
    *lat = a;
    *lon = b;
    return 1;
}

static void draw_coordinate_prompt_bg(vita2d_pgf *font, double lat, double lon, const char *message) {
    vita2d_start_drawing();
    vita2d_clear_screen();
    vita2d_draw_rectangle(85, 70, 790, 395, COL_PANEL);
    btext(font, 120, 120, 1.02f, COL_TEXT, "SEARCH COORDINATES");
    btext(font, 120, 168, 0.76f, COL_TEXT, "TYPE: LATITUDE,LONGITUDE");
    btext(font, 120, 205, 0.68f, COL_DIM, "EXAMPLE: 40.712800,-74.006000");
    btextf(font, 120, 254, 0.68f, COL_DIM, "CURRENT: %.6f, %.6f", lat, lon);
    if (message && *message) btext(font, 120, 302, 0.72f, COL_SELECTED, message);
    btext(font, 120, 408, 0.68f, COL_TEXT, "X = ENTER     CIRCLE = CANCEL");
    vita2d_end_drawing();
}

static int coordinate_keyboard(vita2d_pgf *font, double current_lat, double current_lon,
                               double *out_lat, double *out_lon) {
    char initial_ascii[64];
    snprintf(initial_ascii, sizeof(initial_ascii), "%.6f,%.6f", current_lat, current_lon);

    for (;;) {
        SceWChar16 title[64];
        SceWChar16 initial[64];
        SceWChar16 input[64];
        memset(input, 0, sizeof(input));
        ascii_to_utf16("Coordinates: latitude,longitude", title, 64);
        ascii_to_utf16(initial_ascii, initial, 64);

        SceImeDialogParam param;
        sceImeDialogParamInit(&param);
        param.supportedLanguages = SCE_IME_LANGUAGE_ENGLISH;
        param.languagesForced = SCE_TRUE;
        param.type = SCE_IME_TYPE_BASIC_LATIN;
        param.option = SCE_IME_OPTION_NO_AUTO_CAPITALIZATION | SCE_IME_OPTION_NO_ASSISTANCE;
        param.dialogMode = SCE_IME_DIALOG_DIALOG_MODE_WITH_CANCEL;
        param.textBoxMode = SCE_IME_DIALOG_TEXTBOX_MODE_WITH_CLEAR;
        param.title = title;
        param.maxTextLength = 63;
        param.initialText = initial;
        param.inputTextBuffer = input;
        param.enterLabel = SCE_IME_ENTER_LABEL_GO;

        int rc = sceImeDialogInit(&param);
        if (rc < 0) {
            snprintf(g_status, sizeof(g_status), "KEYBOARD ERROR 0x%08X", (unsigned int)rc);
            return -1;
        }

        for (;;) {
            draw_coordinate_prompt_bg(font, current_lat, current_lon, "USE THE VITA KEYBOARD");
            vita2d_common_dialog_update();
            vita2d_swap_buffers();

            if (sceImeDialogGetStatus() == SCE_COMMON_DIALOG_STATUS_FINISHED) {
                SceImeDialogResult result;
                memset(&result, 0, sizeof(result));
                sceImeDialogGetResult(&result);
                sceImeDialogTerm();

                if (result.button != SCE_IME_DIALOG_BUTTON_ENTER) return 0;

                char typed[80];
                utf16_to_ascii(input, typed, sizeof(typed));
                double lat = 0.0, lon = 0.0;
                if (parse_coordinates(typed, &lat, &lon)) {
                    *out_lat = lat;
                    *out_lon = lon;
                    return 1;
                }

                snprintf(initial_ascii, sizeof(initial_ascii), "%s", typed);
                draw_coordinate_prompt_bg(font, current_lat, current_lon,
                                          "INVALID - USE LATITUDE,LONGITUDE");
                vita2d_swap_buffers();
                sceKernelDelayThread(900000);
                break;
            }
            sceKernelDelayThread(16000);
        }
    }
}

static int select_from_touch(int screen_x, int screen_y) {
    if (screen_x < (int)MAP_X || screen_x > (int)(MAP_X + MAP_W) ||
        screen_y < (int)MAP_Y || screen_y > (int)(MAP_Y + MAP_H)) return 0;
    float best = 28.0f;
    int best_index = -1;
    for (int i = 0; i < g_aircraft_count; i++) {
        float x, y;
        aircraft_screen_pos(&g_aircraft[i], &x, &y);
        float dx = x - (float)screen_x, dy = y - (float)screen_y;
        float d = sqrtf(dx * dx + dy * dy);
        if (d < best) { best = d; best_index = i; }
    }
    if (best_index >= 0 && best_index != g_selected) {
        g_selected = best_index;
        return 1;
    }
    return 0;
}

static void load_current_map(vita2d_pgf *font, const AppConfig *cfg) {
    (void)font;
    /* v1.8 never blanks the display while rebuilding the map. Existing cached
       tiles stay visible and missing tiles stream in progressively. */
    map_load_for_location_zoom(&g_map, cfg->latitude, cfg->longitude, cfg->range_km, g_map_zoom,
                               MAP_X, MAP_Y, MAP_W, MAP_H,
                               g_map_status, sizeof(g_map_status));
}

static int visible_range_from_map(void) {
    int km = (int)ceil(map_visible_radius_km(&g_map));
    if (km < 1) km = 1;
    if (km > 460) km = 460;
    return km;
}

static void commit_map_center(AppConfig *cfg, int *net_ok, int refresh_now) {
    if (!cfg) return;
    cfg->latitude = g_map.center_lat;
    cfg->longitude = g_map.center_lon;
    cfg->range_km = visible_range_from_map();
    config_save(cfg);
    if (refresh_now) {
        g_selected = -1;
        ensure_network_and_refresh(cfg, net_ok);
        refresh_route_for_selection();
    }
}

static void finish_pinch_zoom(AppConfig *cfg, int *net_ok) {
    if (!cfg) return;

    /* Rebase to a sharper integer tile level without downloading synchronously.
       Old cached imagery remains visible while map_update() streams new tiles. */
    map_rebase_high_quality(&g_map, cfg->latitude, cfg->longitude, cfg->range_km,
                            g_map_status, sizeof(g_map_status));
    g_map_zoom = g_map.zoom;
    commit_map_center(cfg, net_ok, 1);
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    COL_BG       = RGBA8(8, 13, 16, 255);
    COL_PANEL    = RGBA8(18, 27, 34, 248);
    COL_TEXT     = RGBA8(245, 250, 249, 255);
    COL_DIM      = RGBA8(213, 230, 230, 255);
    COL_ACCENT   = RGBA8(70, 239, 166, 255);
    COL_SELECTED = RGBA8(89, 222, 255, 255);
    COL_CARD     = RGBA8(30, 43, 51, 255);

    AppConfig cfg;
    int config_result = config_load(&cfg);
    if (config_result != 0) apply_packaged_location(&cfg);
    cfg.refresh_seconds = 4;
    config_save(&cfg);
    g_map_zoom = map_recommended_zoom(cfg.range_km);

    route_clear(&g_route);
    map_state_init(&g_map);

    SceAppUtilInitParam app_init;
    SceAppUtilBootParam boot_param;
    memset(&app_init, 0, sizeof(app_init));
    memset(&boot_param, 0, sizeof(boot_param));
    sceAppUtilInit(&app_init, &boot_param);
    SceCommonDialogConfigParam dialog_cfg;
    memset(&dialog_cfg, 0, sizeof(dialog_cfg));
    sceCommonDialogSetConfigParam(&dialog_cfg);
    sceSysmoduleLoadModule(SCE_SYSMODULE_IME);

    vita2d_init();
    vita2d_set_clear_color(COL_BG);
    vita2d_set_vblank_wait(1);
    vita2d_pgf *font = vita2d_load_default_pgf();
    if (!font) {
        vita2d_fini();
        sceKernelExitProcess(1);
        return 1;
    }

    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);

    draw_loading(font, "CONNECTING TO LIVE FLIGHT DATA...");
    char net_error[128];
    int net_ok = vfr_net_init(net_error, sizeof(net_error));
    if (net_ok >= 0) {
        refresh_flights(&cfg);
        refresh_route_for_selection();
        map_load_for_location_zoom(&g_map, cfg.latitude, cfg.longitude, cfg.range_km, g_map_zoom,
                                   MAP_X, MAP_Y, MAP_W, MAP_H, g_map_status, sizeof(g_map_status));
    } else {
        snprintf(g_status, sizeof(g_status), "NETWORK INIT ERROR");
        map_load_for_location_zoom(&g_map, cfg.latitude, cfg.longitude, cfg.range_km, g_map_zoom,
                                   MAP_X, MAP_Y, MAP_W, MAP_H, g_map_status, sizeof(g_map_status));
    }

    /* From v1.7 onward, aircraft range is derived from the actual map viewport. */
    {
        int visible_km = visible_range_from_map();
        if (visible_km != cfg.range_km) {
            cfg.range_km = visible_km;
            config_save(&cfg);
            if (net_ok >= 0) {
                refresh_flights(&cfg);
                refresh_route_for_selection();
            }
        }
    }

    SceCtrlData pad, old_pad;
    memset(&pad, 0, sizeof(pad));
    memset(&old_pad, 0, sizeof(old_pad));
    int touch_mode = 0; /* 0 none, 1 single-finger map gesture, 2 pinch */
    int touch_start_x = 0, touch_start_y = 0;
    int touch_last_x = 0, touch_last_y = 0;
    int touch_moved = 0;
    int long_press_done = 0;
    SceUInt64 touch_start_us = 0;
    SceUInt64 last_touch_activity_us = sceKernelGetProcessTimeWide();
    float last_pinch_distance = 0.0f;
    int running = 1;

    while (running) {
        sceCtrlPeekBufferPositive(0, &pad, 1);
        unsigned int pressed = pad.buttons & ~old_pad.buttons;


        if (pressed & SCE_CTRL_UP) {
            if (g_aircraft_count > 0) {
                g_selected--;
                if (g_selected < 0) g_selected = g_aircraft_count - 1;
                refresh_route_for_selection();
            }
        }
        if (pressed & SCE_CTRL_DOWN) {
            if (g_aircraft_count > 0) {
                g_selected++;
                if (g_selected >= g_aircraft_count) g_selected = 0;
                refresh_route_for_selection();
            }
        }
        if (pressed & SCE_CTRL_SQUARE) g_auto_refresh = !g_auto_refresh;
        if (pressed & SCE_CTRL_START) {
            char old_key[24];
            snprintf(old_key, sizeof(old_key), "%s", g_selected >= 0 ? g_aircraft[g_selected].callsign : "");
            ensure_network_and_refresh(&cfg, &net_ok);
            if (g_selected >= 0 && strcmp(old_key, g_aircraft[g_selected].callsign)) refresh_route_for_selection();
        }

        if (pressed & SCE_CTRL_TRIANGLE) {
            double new_lat = cfg.latitude, new_lon = cfg.longitude;
            int search_result = coordinate_keyboard(font, cfg.latitude, cfg.longitude, &new_lat, &new_lon);
            if (search_result > 0) {
                cfg.latitude = new_lat;
                cfg.longitude = new_lon;
                config_save(&cfg);
                if (g_map_zoom < 1) g_map_zoom = map_recommended_zoom(cfg.range_km);
                load_current_map(font, &cfg);
                cfg.range_km = visible_range_from_map();
                config_save(&cfg);
                g_selected = -1;
                ensure_network_and_refresh(&cfg, &net_ok);
                refresh_route_for_selection();
                snprintf(g_status, sizeof(g_status), "TRACKING %.4f, %.4f", cfg.latitude, cfg.longitude);
            }
            sceCtrlPeekBufferPositive(0, &pad, 1);
            old_pad = pad;
        }

        SceTouchData touch;
        memset(&touch, 0, sizeof(touch));
        int touch_down = 0;
        int reports = 0;
        if (sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1) > 0) reports = touch.reportNum;
        if (reports > 0) {
            touch_down = 1;
            last_touch_activity_us = sceKernelGetProcessTimeWide();

            if (reports >= 2) {
                float x1 = (float)touch.report[0].x * 0.5f;
                float y1 = (float)touch.report[0].y * 0.5f;
                float x2 = (float)touch.report[1].x * 0.5f;
                float y2 = (float)touch.report[1].y * 0.5f;
                float dx = x2 - x1, dy = y2 - y1;
                float dist = sqrtf(dx * dx + dy * dy);
                float midx = (x1 + x2) * 0.5f;
                float midy = (y1 + y2) * 0.5f;

                if (midx >= MAP_X && midx <= MAP_X + MAP_W && midy >= MAP_Y && midy <= MAP_Y + MAP_H) {
                    if (touch_mode != 2) {
                        touch_mode = 2;
                        last_pinch_distance = dist;
                        touch_moved = 1;
                    } else if (last_pinch_distance > 8.0f && dist > 8.0f) {
                        float factor = dist / last_pinch_distance;
                        if (factor < 0.88f) factor = 0.88f;
                        if (factor > 1.14f) factor = 1.14f;
                        map_visual_zoom(&g_map, factor, midx, midy);
                        last_pinch_distance = dist;
                    }
                }
            } else if (touch_mode != 2) {
                int sx = touch.report[0].x / 2;
                int sy = touch.report[0].y / 2;
                if (touch_mode == 0) {
                    touch_mode = 1;
                    touch_start_x = touch_last_x = sx;
                    touch_start_y = touch_last_y = sy;
                    touch_start_us = sceKernelGetProcessTimeWide();
                    touch_moved = 0;
                    long_press_done = 0;
                    last_pinch_distance = 0.0f;
                } else if (touch_mode == 1) {
                    int dx = sx - touch_last_x;
                    int dy = sy - touch_last_y;
                    int total_dx = sx - touch_start_x;
                    int total_dy = sy - touch_start_y;
                    if (sx >= (int)MAP_X && sx <= (int)(MAP_X + MAP_W) &&
                        sy >= (int)MAP_Y && sy <= (int)(MAP_Y + MAP_H)) {
                        if (dx || dy) map_pan_pixels(&g_map, (float)dx, (float)dy);
                        if (total_dx * total_dx + total_dy * total_dy > 36) touch_moved = 1;
                    }
                    touch_last_x = sx;
                    touch_last_y = sy;

                    /* Stationary hold: relocate the live tracking center to the
                       geographic point directly under the finger. */
                    if (!touch_moved && !long_press_done &&
                        sceKernelGetProcessTimeWide() - touch_start_us >= 650000ULL &&
                        sx >= (int)MAP_X && sx <= (int)(MAP_X + MAP_W) &&
                        sy >= (int)MAP_Y && sy <= (int)(MAP_Y + MAP_H)) {
                        double lat = 0.0, lon = 0.0;
                        map_screen_to_geo(&g_map, (float)sx, (float)sy, &lat, &lon);
                        map_set_center(&g_map, lat, lon);
                        commit_map_center(&cfg, &net_ok, 1);
                        snprintf(g_status, sizeof(g_status), "RECENTERED %.4f, %.4f", cfg.latitude, cfg.longitude);
                        long_press_done = 1;
                    }
                }
            }
        } else if (touch_mode != 0) {
            if (touch_mode == 2) {
                finish_pinch_zoom(&cfg, &net_ok);
            } else if (touch_mode == 1) {
                if (touch_moved) {
                    /* Panning is live and smooth; network/plane refresh happens
                       only once the finger is released. */
                    commit_map_center(&cfg, &net_ok, 1);
                    snprintf(g_status, sizeof(g_status), "TRACKING MAP CENTER");
                } else if (!long_press_done) {
                    if (select_from_touch(touch_start_x, touch_start_y)) refresh_route_for_selection();
                }
            }
            touch_mode = 0;
            touch_moved = 0;
            long_press_done = 0;
            last_pinch_distance = 0.0f;
            last_touch_activity_us = sceKernelGetProcessTimeWide();
        }

        if (g_auto_refresh) {
            static SceUInt64 last_auto_attempt_us = 0;
            SceUInt64 now = sceKernelGetProcessTimeWide();
            SceUInt64 anchor = g_last_refresh_us > last_auto_attempt_us ? g_last_refresh_us : last_auto_attempt_us;
            if (anchor == 0 || now - anchor >= (SceUInt64)cfg.refresh_seconds * 1000000ULL) {
                last_auto_attempt_us = now;
                char old_key[24];
                snprintf(old_key, sizeof(old_key), "%s", g_selected >= 0 ? g_aircraft[g_selected].callsign : "");
                ensure_network_and_refresh(&cfg, &net_ok);
                if (g_selected >= 0 && strcmp(old_key, g_aircraft[g_selected].callsign)) refresh_route_for_selection();
            }
            now = sceKernelGetProcessTimeWide();
            if (g_selected >= 0 && (g_last_route_refresh_us == 0 || now - g_last_route_refresh_us >= 60000000ULL))
                refresh_route_for_selection();
        }

        /* Never perform a satellite network request while a finger is on the
           screen. Missing tiles stream one at a time after the gesture, keeping
           drag/pinch interaction responsive. */
        if (!touch_down && touch_mode == 0) {
            static SceUInt64 last_tile_step_us = 0;
            SceUInt64 tile_now = sceKernelGetProcessTimeWide();
            if (tile_now - last_touch_activity_us >= 180000ULL &&
                tile_now - last_tile_step_us >= 70000ULL) {
                last_tile_step_us = tile_now;
                map_update(&g_map, g_map_status, sizeof(g_map_status));
            }
        }

        draw_ui(font, &cfg);
        old_pad = pad;
        sceKernelDelayThread(12000);
    }

    map_free(&g_map);
    vfr_net_term();
    vita2d_free_pgf(font);
    vita2d_fini();
    sceSysmoduleUnloadModule(SCE_SYSMODULE_IME);
    sceAppUtilShutdown();
    sceKernelExitProcess(0);
    return 0;
}
