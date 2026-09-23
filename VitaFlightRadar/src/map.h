#ifndef VFR_MAP_H
#define VFR_MAP_H

#include <stddef.h>
#include <vita2d.h>

#define VFR_MAP_MAX_TILES 40

typedef struct MapTile {
    int x;
    int y;
    int zoom;
    unsigned int last_used;
    vita2d_texture *texture;
} MapTile;

typedef struct MapState {
    double center_lat;
    double center_lon;
    double center_pixel_x;
    double center_pixel_y;
    int radius_km;
    int zoom;
    float view_x;
    float view_y;
    float view_w;
    float view_h;
    int tile_count;
    int loaded_count;
    float visual_scale;
    float visual_offset_x;
    float visual_offset_y;
    unsigned int tick;
    MapTile tiles[VFR_MAP_MAX_TILES];
} MapState;

void map_state_init(MapState *map);
void map_free(MapState *map);
int map_recommended_zoom(int radius_km);
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
                               size_t status_size);
int map_load_for_location(MapState *map,
                          double lat,
                          double lon,
                          int radius_km,
                          float view_x,
                          float view_y,
                          float view_w,
                          float view_h,
                          char *status,
                          size_t status_size);

/* Progressive map update. Loads at most one missing tile per call and keeps
   already-loaded tiles visible, so gestures never trigger a full blank reload. */
int map_update(MapState *map, char *status, size_t status_size);

void map_draw(MapState *map);
void map_visual_zoom(MapState *map, float factor, float anchor_x, float anchor_y);
void map_reset_visual_zoom(MapState *map);
void map_pan_pixels(MapState *map, float dx, float dy);
void map_set_center(MapState *map, double lat, double lon);
void map_geo_to_screen(const MapState *map, double lat, double lon, float *x, float *y);
void map_screen_to_geo(const MapState *map, float x, float y, double *lat, double *lon);
double map_visible_radius_km(const MapState *map);
int map_rebase_high_quality(MapState *map,
                            double lat,
                            double lon,
                            int radius_km,
                            char *status,
                            size_t status_size);

#endif
