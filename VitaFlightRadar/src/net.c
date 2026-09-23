#include "net.h"

#include <curl/curl.h>
#include <psp2/net/http.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/sysmodule.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NET_MEMORY_SIZE   (4 * 1024 * 1024)
#define HTTP_MEMORY_SIZE  (4 * 1024 * 1024)
#define MAX_RESPONSE_SIZE (8 * 1024 * 1024)

typedef struct MemoryBuffer {
    char *data;
    size_t size;
    size_t capacity;
    int overflowed;
} MemoryBuffer;

static void *g_net_memory = NULL;
static int g_net_module_loaded = 0;
static int g_net_initialized = 0;
static int g_netctl_initialized = 0;
static int g_http_module_loaded = 0;
static int g_http_initialized = 0;
static int g_curl_initialized = 0;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    MemoryBuffer *mem = (MemoryBuffer *)userp;
    size_t bytes = size * nmemb;
    if (!mem || bytes == 0) return bytes;

    if (mem->size + bytes > MAX_RESPONSE_SIZE) {
        mem->overflowed = 1;
        return 0;
    }

    size_t needed = mem->size + bytes + 1;
    if (needed > mem->capacity) {
        size_t next = mem->capacity ? mem->capacity : 16384;
        while (next < needed && next < MAX_RESPONSE_SIZE + 1) next *= 2;
        if (next > MAX_RESPONSE_SIZE + 1) next = MAX_RESPONSE_SIZE + 1;
        if (next < needed) {
            mem->overflowed = 1;
            return 0;
        }
        char *grown = (char *)realloc(mem->data, next);
        if (!grown) return 0;
        mem->data = grown;
        mem->capacity = next;
    }

    memcpy(mem->data + mem->size, contents, bytes);
    mem->size += bytes;
    mem->data[mem->size] = '\0';
    return bytes;
}

void vfr_net_term(void) {
    if (g_curl_initialized) {
        curl_global_cleanup();
        g_curl_initialized = 0;
    }
    if (g_http_initialized) {
        sceHttpTerm();
        g_http_initialized = 0;
    }
    if (g_http_module_loaded) {
        sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTP);
        g_http_module_loaded = 0;
    }
    if (g_netctl_initialized) {
        sceNetCtlTerm();
        g_netctl_initialized = 0;
    }
    if (g_net_initialized) {
        sceNetTerm();
        g_net_initialized = 0;
    }
    if (g_net_module_loaded) {
        sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
        g_net_module_loaded = 0;
    }
    free(g_net_memory);
    g_net_memory = NULL;
}

static int init_fail(int rc, char *error_text, size_t error_text_size, const char *what) {
    if (error_text && error_text_size)
        snprintf(error_text, error_text_size, "%s: 0x%08X", what ? what : "Network init failed", rc);
    vfr_net_term();
    return rc;
}

int vfr_net_init(char *error_text, size_t error_text_size) {
    if (error_text && error_text_size) error_text[0] = '\0';
    if (g_curl_initialized) return 0;
    vfr_net_term();

    int rc = sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
    if (rc < 0) return init_fail(rc, error_text, error_text_size, "NET module");
    g_net_module_loaded = 1;

    g_net_memory = malloc(NET_MEMORY_SIZE);
    if (!g_net_memory) return init_fail(-1, error_text, error_text_size, "Network memory");

    SceNetInitParam param;
    memset(&param, 0, sizeof(param));
    param.memory = g_net_memory;
    param.size = NET_MEMORY_SIZE;
    param.flags = 0;
    rc = sceNetInit(&param);
    if (rc < 0) return init_fail(rc, error_text, error_text_size, "sceNetInit");
    g_net_initialized = 1;

    rc = sceNetCtlInit();
    if (rc < 0) return init_fail(rc, error_text, error_text_size, "sceNetCtlInit");
    g_netctl_initialized = 1;

    /* VitaSDK's libcurl sample initializes the HTTP module even though TLS is
       handled by libcurl/OpenSSL. Keep that known-good setup for both Vita 1000
       and Vita 2000 systems. */
    rc = sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);
    if (rc < 0) return init_fail(rc, error_text, error_text_size, "HTTP module");
    g_http_module_loaded = 1;

    rc = sceHttpInit(HTTP_MEMORY_SIZE);
    if (rc < 0) return init_fail(rc, error_text, error_text_size, "sceHttpInit");
    g_http_initialized = 1;

    CURLcode crc = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (crc != CURLE_OK) {
        if (error_text && error_text_size)
            snprintf(error_text, error_text_size, "curl init: %s", curl_easy_strerror(crc));
        vfr_net_term();
        return -2;
    }
    g_curl_initialized = 1;
    return 0;
}

static int http_request(const char *url,
                        const char *post_body,
                        const char *content_type,
                        char **response_body,
                        size_t *response_size,
                        long *http_status,
                        char *error_text,
                        size_t error_text_size) {
    if (error_text && error_text_size) error_text[0] = '\0';
    if (!url || !response_body || !response_size || !http_status) return -1;
    if (!g_curl_initialized) {
        if (error_text && error_text_size) snprintf(error_text, error_text_size, "Network is not initialized");
        return -2;
    }

    *response_body = NULL;
    *response_size = 0;
    *http_status = 0;

    CURL *curl = curl_easy_init();
    if (!curl) return -3;

    MemoryBuffer mem;
    memset(&mem, 0, sizeof(mem));
    mem.data = (char *)malloc(1);
    if (!mem.data) {
        curl_easy_cleanup(curl);
        return -4;
    }
    mem.data[0] = '\0';
    mem.capacity = 1;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: */*");
    if (content_type && *content_type) {
        char header[96];
        snprintf(header, sizeof(header), "Content-Type: %s", content_type);
        headers = curl_slist_append(headers, header);
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "VitaFlightRadar/1.9.3 (PS Vita homebrew)");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 18L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

    /* Do not depend on the firmware's native HTTPS/TLS implementation. The
       VitaSDK libcurl/OpenSSL path negotiates TLS 1.2 itself, which matters on
       older homebrew firmware commonly found on PCH-1000 systems. */
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    if (post_body) {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_body);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(post_body));
    }

    CURLcode result = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, http_status);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        if (error_text && error_text_size)
            snprintf(error_text, error_text_size, "curl: %.80s", curl_easy_strerror(result));
        free(mem.data);
        return -5;
    }
    if (mem.overflowed) {
        if (error_text && error_text_size) snprintf(error_text, error_text_size, "Response too large");
        free(mem.data);
        return -6;
    }

    *response_body = mem.data;
    *response_size = mem.size;
    return 0;
}

int vfr_http_get(const char *url,
                 char **response_body,
                 size_t *response_size,
                 long *http_status,
                 char *error_text,
                 size_t error_text_size) {
    return http_request(url, NULL, NULL, response_body, response_size, http_status,
                        error_text, error_text_size);
}

int vfr_http_post_json(const char *url,
                       const char *json_body,
                       char **response_body,
                       size_t *response_size,
                       long *http_status,
                       char *error_text,
                       size_t error_text_size) {
    return http_request(url, json_body ? json_body : "{}", "application/json",
                        response_body, response_size, http_status,
                        error_text, error_text_size);
}

void vfr_http_free(char *response_body) {
    free(response_body);
}
