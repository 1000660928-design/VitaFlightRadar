#include "map.h"
#include "net.h"

#include <math.h>
#include <psp2/io/stat.h>
#include <psp2/kernel/processmgr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define TILE_SIZE 256.0
#define MIN_ZOOM 1
#define MAX_ZOOM 21
#define CACHE_DIR "ux0:data/VitaFlightRadar/map_cache"

#define APP_DATA_DIR "ux0:data/VitaFlightRadar"

static double clamp_lat(double lat) {
    if (lat > 85.05112878) return 85.05112878;
    if (lat < -85.05112878) return -85.05112878;
    return lat;
}

static double wrap_lon(double lon) {
    while (lon > 180.0) lon -= 360.0;
    while (lon < -180.0) lon += 360.0;
    return lon;
}

static double world_size(int zoom) {
    return (double)(1u << zoom) * TILE_SIZE;
}

static double lon_to_world_x(double lon, int zoom) {
    return (wrap_lon(lon) + 180.0) / 360.0 * world_size(zoom);
}

static double lat_to_world_y(double lat, int zoom) {
    double r = clamp_lat(lat) * M_PI / 180.0;
    double y = (1.0 - log(tan(r) + 1.0 / cos(r)) / M_PI) / 2.0;
    return y * world_size(zoom);
}

static double world_x_to_lon(double x, int zoom) {
    double w = world_size(zoom);
    while (x < 0.0) x += w;
    while (x >= w) x -= w;
    return x / w * 360.0 - 180.0;
}

static double world_y_to_lat(double y, int zoom) {
    double w = world_size(zoom);
    if (y < 0.0) y = 0.0;
    if (y > w) y = w;
    double n = M_PI - 2.0 * M_PI * y / w;
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

int map_recommended_zoom(int radius_km) {
    if (radius_km <= 2) return 15;
    if (radius_km <= 5) return 14;
    if (radius_km <= 10) return 13;
    if (radius_km <= 20) return 12;
    if (radius_km <= 45) return 11;
    if (radius_km <= 90) return 10;
    if (radius_km <= 180) return 9;
    if (radius_km <= 360) return 8;
    return 7;
}

void map_state_init(MapState *map) {
    if (!map) return;
    memset(map, 0, sizeof(*map));
    map->visual_scale = 1.0f;
    map->zoom = 13;
    /* PCH-1000 has no built-in user storage. Make the app/cache directory
       explicitly so official-memory-card and SD2Vita ux0 layouts behave the
       same as the PCH-2000 internal-storage setup. */
    sceIoMkdir(APP_DATA_DIR, 0777);
    sceIoMkdir(CACHE_DIR, 0777);
}

void map_free(MapState *map) {
    if (!map) return;
    for (int i = 0; i < VFR_MAP_MAX_TILES; i++) {
        if (map->tiles[i].texture) {
            vita2d_free_texture(map->tiles[i].texture);
            map->tiles[i].texture = NULL;
        }
    }
    map->tile_count = 0;
    map->loaded_count = 0;
}

static vita2d_texture *decode_tile(const void *data, size_t size) {
    if (!data || size < 8) return NULL;
    const unsigned char *b = (const unsigned char *)data;
    if (b[0] == 0xFF && b[1] == 0xD8)
        return vita2d_load_JPEG_buffer(data, (unsigned long)size);
    if (b[0] == 0x89 && b[1] == 'P' && b[2] == 'N' && b[3] == 'G')
        return vita2d_load_PNG_buffer(data);
    return NULL;
}

static void tile_cache_path(char *out, size_t out_size, int z, int x, int y) {
    snprintf(out, out_size, CACHE_DIR "/%d_%d_%d.tile", z, x, y);
}

static int load_cached_bytes(int z, int x, int y, char **out, size_t *out_size) {
    char path[160];
    tile_cache_path(path, sizeof(path), z, x, y);
    FILE *fp = fopen(path, "rb");
    if (!fp) return 0;
    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); return 0; }
    long len = ftell(fp);
    if (len <= 64 || len > 2 * 1024 * 1024) { fclose(fp); return 0; }
    rewind(fp);
    char *buf = (char *)malloc((size_t)len);
    if (!buf) { fclose(fp); return 0; }
    size_t got = fread(buf, 1, (size_t)len, fp);
    fclose(fp);
    if (got != (size_t)len) { free(buf); return 0; }
    *out = buf;
    *out_size = got;
    return 1;
}

static void save_cached_bytes(int z, int x, int y, const void *data, size_t size) {
    if (!data || size <= 64) return;
    char path[160];
    tile_cache_path(path, sizeof(path), z, x, y);
    FILE *fp = fopen(path, "wb");
    if (!fp) return;
    fwrite(data, 1, size, fp);
    fclose(fp);
}

static vita2d_texture *fetch_tile_texture(int z, int x, int y, long *last_http, int *last_rc, int *from_cache) {
    char *cached = NULL;
    size_t cached_size = 0;
    if (load_cached_bytes(z, x, y, &cached, &cached_size)) {
        vita2d_texture *tex = decode_tile(cached, cached_size);
        free(cached);
        if (tex) {
            if (from_cache) *from_cache = 1;
            return tex;
        }
    }

    static const char *hosts[] = {
        "https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile",
        "https://clarity.maptiles.arcgis.com/ArcGIS/rest/services/World_Imagery/MapServer/tile"
    };

    char url[320];
    for (int attempt = 0; attempt < 2; attempt++) {
        snprintf(url, sizeof(url), "%s/%d/%d/%d", hosts[attempt], z, y, x);
        char *body = NULL;
        size_t body_size = 0;
        long http_status = 0;
        char error[96];
        int rc = vfr_http_get(url, &body, &body_size, &http_status, error, sizeof(error));
        if (last_http) *last_http = http_status;
        if (last_rc) *last_rc = rc;
        if (rc >= 0 && http_status == 200 && body && body_size > 64) {
            vita2d_texture *tex = decode_tile(body, body_size);
            if (tex) save_cached_bytes(z, x, y, body, body_size);
            vfr_http_free(body);
            if (tex) {
                if (from_cache) *from_cache = 0;
                return tex;
            }
        } else {
            vfr_http_free(body);
        }
    }
    return NULL;
}

static int wrapped_x(int x, int z) {
    int n = 1 << z;
    int w = x % n;
    if (w < 0) w += n;
    return w;
}

static MapTile *find_tile(MapState *map, int z, int x, int y) {
    if (!map) return NULL;
    int wx = wrapped_x(x, z);
    for (int i = 0; i < VFR_MAP_MAX_TILES; i++) {
        MapTile *t = &map->tiles[i];
        if (t->texture && t->zoom == z && t->x == wx && t->y == y) return t;
    }
    return NULL;
}

static int tile_is_in_current_need(const MapState *map, const MapTile *t) {
    if (!map || !t || !t->texture || t->zoom != map->zoom) return 0;
    const double scale = map->visual_scale > 0.01f ? map->visual_scale : 1.0f;
    double half_w = (double)map->view_w * 0.5 / scale + TILE_SIZE * 1.2;
    double half_h = (double)map->view_h * 0.5 / scale + TILE_SIZE * 1.2;
    int min_x = (int)floor((map->center_pixel_x - half_w) / TILE_SIZE);
    int max_x = (int)floor((map->center_pixel_x + half_w) / TILE_SIZE);
    int min_y = (int)floor((map->center_pixel_y - half_h) / TILE_SIZE);
    int max_y = (int)floor((map->center_pixel_y + half_h) / TILE_SIZE);
    if (t->y < min_y || t->y > max_y) return 0;
    for (int tx = min_x; tx <= max_x; tx++)
        if (wrapped_x(tx, map->zoom) == t->x) return 1;
    return 0;
}

static MapTile *alloc_tile_slot(MapState *map) {
    for (int i = 0; i < VFR_MAP_MAX_TILES; i++)
        if (!map->tiles[i].texture) return &map->tiles[i];

    int best = -1;
    unsigned int oldest = 0xFFFFFFFFu;
    for (int i = 0; i < VFR_MAP_MAX_TILES; i++) {
        if (tile_is_in_current_need(map, &map->tiles[i])) continue;
        if (map->tiles[i].last_used < oldest) {
            oldest = map->tiles[i].last_used;
            best = i;
        }
    }
    if (best < 0) {
        for (int i = 0; i < VFR_MAP_MAX_TILES; i++) {
            if (map->tiles[i].last_used < oldest) {
                oldest = map->tiles[i].last_used;
                best = i;
            }
        }
    }
    if (best < 0) return NULL;
    if (map->tiles[best].texture) vita2d_free_texture(map->tiles[best].texture);
    memset(&map->tiles[best], 0, sizeof(map->tiles[best]));
    return &map->tiles[best];
}

static void normalize_center(MapState *map) {
    if (!map) return;
    double w = world_size(map->zoom);
    while (map->center_pixel_x < 0.0) map->center_pixel_x += w;
    while (map->center_pixel_x >= w) map->center_pixel_x -= w;

    double scale = map->visual_scale > 0.01f ? map->visual_scale : 1.0f;
    double half_h = (double)map->view_h * 0.5 / scale;
    if (w <= half_h * 2.0) map->center_pixel_y = w * 0.5;
    else {
        if (map->center_pixel_y < half_h) map->center_pixel_y = half_h;
        if (map->center_pixel_y > w - half_h) map->center_pixel_y = w - half_h;
    }
    map->center_lon = world_x_to_lon(map->center_pixel_x, map->zoom);
    map->center_lat = world_y_to_lat(map->center_pixel_y, map->zoom);
}

void map_set_center(MapState *map, double lat, double lon) {
    if (!map) return;
    map->center_lat = clamp_lat(lat);
    map->center_lon = wrap_lon(lon);
    map->center_pixel_x = lon_to_world_x(map->center_lon, map->zoom);
    map->center_pixel_y = lat_to_world_y(map->center_lat, map->zoom);
    map->visual_offset_x = 0.0f;
    map->visual_offset_y = 0.0f;
    normalize_center(map);
}

int map_load_for_location_zoom(MapState *map,
                               double lat,
                               double lon,
                               int radius_km,
                               int zoom,
                               float view_x,
                               float view_y,
                               float view_w,
                               float view_h,
                               char *status,
                               size_t status_size) {
    if (!map) return -1;
    if (zoom < MIN_ZOOM) zoom = MIN_ZOOM;
    if (zoom > MAX_ZOOM) zoom = MAX_ZOOM;
    map->radius_km = radius_km;
    map->zoom = zoom;
    map->view_x = view_x;
    map->view_y = view_y;
    map->view_w = view_w;
    map->view_h = view_h;
    map->visual_scale = 1.0f;
    map->visual_offset_x = 0.0f;
    map->visual_offset_y = 0.0f;
    map_set_center(map, lat, lon);
    if (status && status_size) snprintf(status, status_size, "SATELLITE STREAMING");
    return 0;
}

int map_load_for_location(MapState *map,
                          double lat,
                          double lon,
                          int radius_km,
                          float view_x,
                          float view_y,
                          float view_w,
                          float view_h,
                          char *status,
                          size_t status_size) {
    return map_load_for_location_zoom(map, lat, lon, radius_km, map_recommended_zoom(radius_km),
                                      view_x, view_y, view_w, view_h, status, status_size);
}

int map_update(MapState *map, char *status, size_t status_size) {
    if (!map || map->zoom < MIN_ZOOM) return -1;
    map->tick++;

    const double scale = map->visual_scale > 0.01f ? map->visual_scale : 1.0f;
    double half_w = (double)map->view_w * 0.5 / scale + TILE_SIZE * 1.1;
    double half_h = (double)map->view_h * 0.5 / scale + TILE_SIZE * 1.1;
    int min_x = (int)floor((map->center_pixel_x - half_w) / TILE_SIZE);
    int max_x = (int)floor((map->center_pixel_x + half_w) / TILE_SIZE);
    int min_y = (int)floor((map->center_pixel_y - half_h) / TILE_SIZE);
    int max_y = (int)floor((map->center_pixel_y + half_h) / TILE_SIZE);
    int n = 1 << map->zoom;
    if (min_y < 0) min_y = 0;
    if (max_y >= n) max_y = n - 1;

    /* Touch all currently useful textures so LRU eviction keeps the visible area. */
    for (int i = 0; i < VFR_MAP_MAX_TILES; i++) {
        if (tile_is_in_current_need(map, &map->tiles[i])) map->tiles[i].last_used = map->tick;
    }

    /* Center-first spiral-ish ordering: choose the nearest missing tile. */
    int cx = (int)floor(map->center_pixel_x / TILE_SIZE);
    int cy = (int)floor(map->center_pixel_y / TILE_SIZE);
    int best_tx = 0, best_ty = 0, best_d = 0x7FFFFFFF;
    int found = 0;
    for (int ty = min_y; ty <= max_y; ty++) {
        for (int tx = min_x; tx <= max_x; tx++) {
            if (find_tile(map, map->zoom, tx, ty)) continue;
            int dx = tx - cx, dy = ty - cy;
            int d = dx * dx + dy * dy;
            if (d < best_d) { best_d = d; best_tx = tx; best_ty = ty; found = 1; }
        }
    }

    if (!found) {
        int loaded_current = 0;
        for (int i = 0; i < VFR_MAP_MAX_TILES; i++)
            if (map->tiles[i].texture && map->tiles[i].zoom == map->zoom && tile_is_in_current_need(map, &map->tiles[i])) loaded_current++;
        if (status && status_size) snprintf(status, status_size, "SATELLITE READY %d TILES", loaded_current);
        return 0;
    }

    static int fail_z = -1, fail_x = -1, fail_y = -1;
    static SceUInt64 retry_after = 0;
    SceUInt64 now = sceKernelGetProcessTimeWide();
    int wx = wrapped_x(best_tx, map->zoom);
    if (fail_z == map->zoom && fail_x == wx && fail_y == best_ty && now < retry_after) return 0;

    long http = 0;
    int rc = 0;
    int from_cache = 0;
    vita2d_texture *tex = fetch_tile_texture(map->zoom, wx, best_ty, &http, &rc, &from_cache);
    int loaded_zoom = map->zoom;
    int loaded_x = wx;
    int loaded_y = best_ty;

    if (!tex) {
        /* Never leave the viewport completely black just because the imagery
           provider has no tile at an aggressive close-zoom level. Walk down
           through up to four parent levels and use the first available image
           as a visual fallback. Higher-resolution children will continue to be
           retried later and paint over it when available. */
        for (int down = 1; down <= 4 && map->zoom - down >= MIN_ZOOM && !tex; down++) {
            int pz = map->zoom - down;
            int px = wx >> down;
            int py = best_ty >> down;
            MapTile *existing = find_tile(map, pz, px, py);
            if (existing && existing->texture) {
                existing->last_used = map->tick;
                if (status && status_size) snprintf(status, status_size, "SATELLITE FALLBACK Z%d", pz);
                fail_z = map->zoom; fail_x = wx; fail_y = best_ty;
                retry_after = now + 5000000ULL;
                return 0;
            }

            long parent_http = 0;
            int parent_rc = 0;
            int parent_cache = 0;
            tex = fetch_tile_texture(pz, px, py, &parent_http, &parent_rc, &parent_cache);
            if (tex) {
                loaded_zoom = pz;
                loaded_x = wrapped_x(px, pz);
                loaded_y = py;
                from_cache = parent_cache;
                if (status && status_size) snprintf(status, status_size, "SATELLITE FALLBACK Z%d", pz);
                break;
            }
        }
    }

    if (!tex) {
        fail_z = map->zoom; fail_x = wx; fail_y = best_ty;
        retry_after = now + 5000000ULL;
        if (status && status_size) {
            if (http > 0) snprintf(status, status_size, "SATELLITE HTTP %ld", http);
            else snprintf(status, status_size, "SATELLITE NET %d", rc);
        }
        return -2;
    }

    vita2d_texture_set_filters(tex, SCE_GXM_TEXTURE_FILTER_LINEAR, SCE_GXM_TEXTURE_FILTER_LINEAR);
    MapTile *slot = alloc_tile_slot(map);
    if (!slot) { vita2d_free_texture(tex); return -3; }
    slot->x = loaded_x;
    slot->y = loaded_y;
    slot->zoom = loaded_zoom;
    slot->texture = tex;
    slot->last_used = map->tick;

    map->loaded_count = 0;
    for (int i = 0; i < VFR_MAP_MAX_TILES; i++) if (map->tiles[i].texture) map->loaded_count++;
    map->tile_count = map->loaded_count;
    if (status && status_size && loaded_zoom == map->zoom)
        snprintf(status, status_size, "%s TILE Z%d", from_cache ? "CACHED" : "SATELLITE", map->zoom);
    return 1;
}

static void draw_tile_at_base_zoom(const MapState *map, const MapTile *tile) {
    if (!map || !tile || !tile->texture) return;
    int dz = map->zoom - tile->zoom;
    if (dz < -2 || dz > 4) return;
    double factor = pow(2.0, (double)dz);
    double tile_world = TILE_SIZE * factor;
    double world_w = world_size(map->zoom);
    double tile_center = ((double)tile->x + 0.5) * tile_world;
    while (tile_center - map->center_pixel_x > world_w * 0.5) tile_center -= world_w;
    while (tile_center - map->center_pixel_x < -world_w * 0.5) tile_center += world_w;
    double tile_left = tile_center - tile_world * 0.5;
    double tile_top = (double)tile->y * tile_world;

    const float cx = map->view_x + map->view_w * 0.5f;
    const float cy = map->view_y + map->view_h * 0.5f;
    const float scale = map->visual_scale > 0.01f ? map->visual_scale : 1.0f;
    float x = cx + (float)(tile_left - map->center_pixel_x) * scale + map->visual_offset_x;
    float y = cy + (float)(tile_top - map->center_pixel_y) * scale + map->visual_offset_y;
    float tex_scale = (float)factor * scale;

    /* Repeat horizontally at very low zoom levels so the slippy map never has
       hard left/right edges when the viewport is wider than one world copy. */
    float repeat = (float)world_w * scale;
    for (int k = -1; k <= 1; k++) {
        float rx = x + repeat * (float)k;
        if (rx + 256.0f * tex_scale < map->view_x - 2.0f || rx > map->view_x + map->view_w + 2.0f) continue;
        vita2d_draw_texture_scale(tile->texture, rx, y, tex_scale, tex_scale);
    }
}

void map_draw(MapState *map) {
    if (!map) return;
    map->tick++;
    vita2d_enable_clipping();
    vita2d_set_clip_rectangle((int)map->view_x, (int)map->view_y,
                              (int)(map->view_x + map->view_w),
                              (int)(map->view_y + map->view_h));

    /* Lower-resolution cached parents are drawn first as seamless fallback;
       current/high-resolution tiles paint over them as they arrive. */
    for (int z = map->zoom - 4; z <= map->zoom + 1; z++) {
        if (z < MIN_ZOOM || z > MAX_ZOOM) continue;
        for (int i = 0; i < VFR_MAP_MAX_TILES; i++) {
            MapTile *tile = &map->tiles[i];
            if (tile->texture && tile->zoom == z) {
                draw_tile_at_base_zoom(map, tile);
                if (z == map->zoom && tile_is_in_current_need(map, tile)) tile->last_used = map->tick;
            }
        }
    }
    vita2d_disable_clipping();
}

void map_visual_zoom(MapState *map, float factor, float anchor_x, float anchor_y) {
    if (!map || factor <= 0.0f) return;
    float old_scale = map->visual_scale > 0.01f ? map->visual_scale : 1.0f;
    float new_scale = old_scale * factor;
    double min_scale = pow(2.0, (double)(MIN_ZOOM - map->zoom));
    double max_scale = pow(2.0, (double)(MAX_ZOOM - map->zoom));
    if (new_scale < (float)min_scale) new_scale = (float)min_scale;
    if (new_scale > (float)max_scale) new_scale = (float)max_scale;
    if (fabsf(new_scale - old_scale) < 0.0005f) return;

    const float cx = map->view_x + map->view_w * 0.5f;
    const float cy = map->view_y + map->view_h * 0.5f;
    float ratio = new_scale / old_scale;
    map->visual_offset_x = anchor_x - cx - (anchor_x - cx - map->visual_offset_x) * ratio;
    map->visual_offset_y = anchor_y - cy - (anchor_y - cy - map->visual_offset_y) * ratio;
    map->visual_scale = new_scale;
}

void map_reset_visual_zoom(MapState *map) {
    if (!map) return;
    map->visual_scale = 1.0f;
    map->visual_offset_x = 0.0f;
    map->visual_offset_y = 0.0f;
}

void map_pan_pixels(MapState *map, float dx, float dy) {
    if (!map) return;
    float scale = map->visual_scale > 0.01f ? map->visual_scale : 1.0f;
    map->center_pixel_x -= (double)dx / (double)scale;
    map->center_pixel_y -= (double)dy / (double)scale;
    normalize_center(map);
}

void map_screen_to_geo(const MapState *map, float x, float y, double *lat, double *lon) {
    if (!map) return;
    const float cx = map->view_x + map->view_w * 0.5f;
    const float cy = map->view_y + map->view_h * 0.5f;
    const float scale = map->visual_scale > 0.01f ? map->visual_scale : 1.0f;
    double px = map->center_pixel_x + (double)(x - cx - map->visual_offset_x) / (double)scale;
    double py = map->center_pixel_y + (double)(y - cy - map->visual_offset_y) / (double)scale;
    if (lon) *lon = world_x_to_lon(px, map->zoom);
    if (lat) *lat = world_y_to_lat(py, map->zoom);
}

void map_geo_to_screen(const MapState *map, double lat, double lon, float *x, float *y) {
    if (!map || !x || !y) return;
    double px = lon_to_world_x(lon, map->zoom);
    double py = lat_to_world_y(lat, map->zoom);
    double w = world_size(map->zoom);
    double dx = px - map->center_pixel_x;
    while (dx > w * 0.5) dx -= w;
    while (dx < -w * 0.5) dx += w;
    double dy = py - map->center_pixel_y;
    const float cx = map->view_x + map->view_w * 0.5f;
    const float cy = map->view_y + map->view_h * 0.5f;
    const float scale = map->visual_scale > 0.01f ? map->visual_scale : 1.0f;
    *x = cx + (float)dx * scale + map->visual_offset_x;
    *y = cy + (float)dy * scale + map->visual_offset_y;
}

double map_visible_radius_km(const MapState *map) {
    if (!map || map->zoom < MIN_ZOOM) return 5.0;
    const double lat_rad = clamp_lat(map->center_lat) * M_PI / 180.0;
    const double scale = map->visual_scale > 0.01f ? (double)map->visual_scale : 1.0;
    const double meters_per_pixel = (156543.03392 * cos(lat_rad)) / ((double)(1u << map->zoom) * scale);
    const double half_w = (double)map->view_w * 0.5;
    const double half_h = (double)map->view_h * 0.5;
    const double corner_pixels = sqrt(half_w * half_w + half_h * half_h);
    double km = corner_pixels * meters_per_pixel / 1000.0;
    if (km < 0.5) km = 0.5;
    if (km > 460.0) km = 460.0; /* adsb.fi point/radius API maximum is 250 NM. */
    return km;
}

int map_rebase_high_quality(MapState *map,
                            double lat,
                            double lon,
                            int radius_km,
                            char *status,
                            size_t status_size) {
    (void)lat;
    (void)lon;
    if (!map || map->zoom < MIN_ZOOM) return -1;

    /* Preserve the geographic point currently under the screen center before
       changing integer tile level. This prevents jumps after pinch gestures. */
    double center_lat = map->center_lat, center_lon = map->center_lon;
    map_screen_to_geo(map,
                      map->view_x + map->view_w * 0.5f,
                      map->view_y + map->view_h * 0.5f,
                      &center_lat, &center_lon);

    const double scale = map->visual_scale > 0.01f ? (double)map->visual_scale : 1.0;
    const double effective_zoom = (double)map->zoom + log(scale) / log(2.0);
    int target_zoom = (int)ceil(effective_zoom - 0.000001);
    if (target_zoom < MIN_ZOOM) target_zoom = MIN_ZOOM;

    /* v1.9.2 quality cushion: once we are at street-level imagery, request
       one complete tile level sharper than the mathematical minimum whenever
       possible. The visual scale compensates for the extra level, so the map
       does not jump; the sharper imagery is downsampled onto the Vita screen
       instead of stretching a lower-resolution tile. */
    if (effective_zoom >= 14.0 && target_zoom < MAX_ZOOM) target_zoom++;
    if (target_zoom > MAX_ZOOM) target_zoom = MAX_ZOOM;

    /* Preserve the exact effective zoom. With the quality cushion residual
       scaling can be below 0.5; that is intentional supersampling, not a zoom
       change. Never upscale the newly requested quality level. */
    double residual = pow(2.0, effective_zoom - (double)target_zoom);
    if (residual < 0.24) residual = 0.24;
    if (residual > 1.00) residual = 1.00;

    map->zoom = target_zoom;
    map->radius_km = radius_km;
    map->visual_scale = (float)residual;
    map->visual_offset_x = 0.0f;
    map->visual_offset_y = 0.0f;
    map->center_lat = center_lat;
    map->center_lon = center_lon;
    map->center_pixel_x = lon_to_world_x(center_lon, map->zoom);
    map->center_pixel_y = lat_to_world_y(center_lat, map->zoom);
    normalize_center(map);

    if (status && status_size) snprintf(status, status_size, "HQ SATELLITE Z%d", map->zoom);
    return 0;
}
