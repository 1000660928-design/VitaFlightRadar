#include "app_config.h"

#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <stdio.h>

#define CONFIG_DIR  "ux0:data/VitaFlightRadar"
#define CONFIG_FILE "ux0:data/VitaFlightRadar/config.txt"

void config_set_defaults(AppConfig *cfg) {
    if (!cfg) return;
    cfg->latitude = 32.0853;
    cfg->longitude = 34.7818;
    cfg->range_km = 5;
    cfg->refresh_seconds = 4;
}

static void normalize(AppConfig *cfg) {
    if (cfg->latitude > 90.0) cfg->latitude = 90.0;
    if (cfg->latitude < -90.0) cfg->latitude = -90.0;
    while (cfg->longitude > 180.0) cfg->longitude -= 360.0;
    while (cfg->longitude < -180.0) cfg->longitude += 360.0;
    if (cfg->range_km < 1) cfg->range_km = 1;
    if (cfg->range_km > 460) cfg->range_km = 460;
    if (cfg->refresh_seconds < 4) cfg->refresh_seconds = 4;
    if (cfg->refresh_seconds > 120) cfg->refresh_seconds = 120;
}

int config_load(AppConfig *cfg) {
    if (!cfg) return -1;
    config_set_defaults(cfg);

    FILE *fp = fopen(CONFIG_FILE, "r");
    if (!fp) return 1;

    char line[128];
    while (fgets(line, sizeof(line), fp)) {
        double d;
        int i;
        if (sscanf(line, "latitude=%lf", &d) == 1) cfg->latitude = d;
        else if (sscanf(line, "longitude=%lf", &d) == 1) cfg->longitude = d;
        else if (sscanf(line, "range_km=%d", &i) == 1) cfg->range_km = i;
        else if (sscanf(line, "refresh_seconds=%d", &i) == 1) cfg->refresh_seconds = i;
    }
    fclose(fp);
    normalize(cfg);
    return 0;
}

int config_save(const AppConfig *cfg) {
    if (!cfg) return -1;
    sceIoMkdir(CONFIG_DIR, 0777);
    FILE *fp = fopen(CONFIG_FILE, "w");
    if (!fp) return -2;
    fprintf(fp, "latitude=%.6f\n", cfg->latitude);
    fprintf(fp, "longitude=%.6f\n", cfg->longitude);
    fprintf(fp, "range_km=%d\n", cfg->range_km);
    fprintf(fp, "refresh_seconds=%d\n", cfg->refresh_seconds);
    fclose(fp);
    return 0;
}
