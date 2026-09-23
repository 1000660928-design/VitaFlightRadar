#ifndef VFR_APP_CONFIG_H
#define VFR_APP_CONFIG_H

typedef struct AppConfig {
    double latitude;
    double longitude;
    int range_km;
    int refresh_seconds;
} AppConfig;

void config_set_defaults(AppConfig *cfg);
int config_load(AppConfig *cfg);
int config_save(const AppConfig *cfg);

#endif
