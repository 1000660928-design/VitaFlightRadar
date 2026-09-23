#ifndef VFR_NET_H
#define VFR_NET_H

#include <stddef.h>

int vfr_net_init(char *error_text, size_t error_text_size);
void vfr_net_term(void);

int vfr_http_get(const char *url,
                 char **response_body,
                 size_t *response_size,
                 long *http_status,
                 char *error_text,
                 size_t error_text_size);

int vfr_http_post_json(const char *url,
                       const char *json_body,
                       char **response_body,
                       size_t *response_size,
                       long *http_status,
                       char *error_text,
                       size_t error_text_size);

void vfr_http_free(char *response_body);

#endif
