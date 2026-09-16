#include "net.h"

#include <psp2/net/http.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/ssl.h>
#include <psp2/sysmodule.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NET_MEMORY_SIZE   (4 * 1024 * 1024)
#define HTTP_MEMORY_SIZE  (4 * 1024 * 1024)
#define SSL_MEMORY_SIZE   (4 * 1024 * 1024)
#define MAX_RESPONSE_SIZE (2 * 1024 * 1024)
#define READ_CHUNK_SIZE   (16 * 1024)

static void *g_net_memory = NULL;
static int g_net_module_loaded = 0;
static int g_https_module_loaded = 0;
static int g_net_initialized = 0;
static int g_netctl_initialized = 0;
static int g_http_initialized = 0;
static int g_ssl_initialized = 0;

void vfr_net_term(void) {
    if (g_ssl_initialized) {
        sceSslTerm();
        g_ssl_initialized = 0;
    }
    if (g_http_initialized) {
        sceHttpTerm();
        g_http_initialized = 0;
    }
    if (g_netctl_initialized) {
        sceNetCtlTerm();
        g_netctl_initialized = 0;
    }
    if (g_net_initialized) {
        sceNetTerm();
        g_net_initialized = 0;
    }
    if (g_https_module_loaded) {
        sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTPS);
        g_https_module_loaded = 0;
    }
    if (g_net_module_loaded) {
        sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
        g_net_module_loaded = 0;
    }
    free(g_net_memory);
    g_net_memory = NULL;
}

static int init_fail(int rc, char *error_text, size_t error_text_size, const char *what) {
    if (error_text && error_text_size) {
        snprintf(error_text, error_text_size, "%s: 0x%08X", what ? what : "Network initialization failed", rc);
    }
    vfr_net_term();
    return rc;
}

int vfr_net_init(char *error_text, size_t error_text_size) {
    if (error_text && error_text_size) error_text[0] = '\0';
    if (g_ssl_initialized) return 0;

    vfr_net_term();

    int rc = sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
    if (rc < 0) return init_fail(rc, error_text, error_text_size, "NET module");
    g_net_module_loaded = 1;

    rc = sceSysmoduleLoadModule(SCE_SYSMODULE_HTTPS);
    if (rc < 0) return init_fail(rc, error_text, error_text_size, "HTTPS module");
    g_https_module_loaded = 1;

    g_net_memory = malloc(NET_MEMORY_SIZE);
    if (!g_net_memory) {
        if (error_text && error_text_size) snprintf(error_text, error_text_size, "Could not allocate network memory");
        vfr_net_term();
        return -1;
    }

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

    rc = sceHttpInit(HTTP_MEMORY_SIZE);
    if (rc < 0) return init_fail(rc, error_text, error_text_size, "sceHttpInit");
    g_http_initialized = 1;

    rc = sceSslInit(SSL_MEMORY_SIZE);
    if (rc < 0) return init_fail(rc, error_text, error_text_size, "sceSslInit");
    g_ssl_initialized = 1;

    /* Public read-only aircraft data; Vita certificate stores can be outdated. */
    sceHttpsDisableOption(SCE_HTTPS_FLAG_SERVER_VERIFY |
                          SCE_HTTPS_FLAG_CN_CHECK |
                          SCE_HTTPS_FLAG_NOT_AFTER_CHECK |
                          SCE_HTTPS_FLAG_NOT_BEFORE_CHECK);
    return 0;
}

int vfr_http_get(const char *url,
                 char **response_body,
                 size_t *response_size,
                 long *http_status,
                 char *error_text,
                 size_t error_text_size) {
    if (error_text && error_text_size) error_text[0] = '\0';
    if (!url || !response_body || !response_size || !http_status) return -1;
    if (!g_ssl_initialized) {
        if (error_text && error_text_size) snprintf(error_text, error_text_size, "Network is not initialized");
        return -2;
    }

    *response_body = NULL;
    *response_size = 0;
    *http_status = 0;

    int tpl = -1, conn = -1, req = -1;
    char *body = NULL;
    size_t used = 0, capacity = 0;
    int rc = -3;

    tpl = sceHttpCreateTemplate("VitaFlightRadar/1.0", 2, 1);
    if (tpl < 0) { rc = tpl; goto fail; }

    sceHttpAddRequestHeader(tpl, "Accept", "application/json", SCE_HTTP_HEADER_ADD);

    conn = sceHttpCreateConnectionWithURL(tpl, url, 0);
    if (conn < 0) { rc = conn; goto fail; }

    req = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_GET, url, 0);
    if (req < 0) { rc = req; goto fail; }

    rc = sceHttpSendRequest(req, NULL, 0);
    if (rc < 0) goto fail;

    int status = 0;
    rc = sceHttpGetStatusCode(req, &status);
    if (rc < 0) goto fail;
    *http_status = status;

    SceUInt64 announced = 0;
    if (sceHttpGetResponseContentLength(req, &announced) >= 0 && announced > 0) {
        if (announced > MAX_RESPONSE_SIZE) {
            if (error_text && error_text_size) snprintf(error_text, error_text_size, "API response was too large");
            rc = -4;
            goto fail;
        }
        capacity = (size_t)announced + 1;
    } else {
        capacity = READ_CHUNK_SIZE + 1;
    }

    body = (char *)malloc(capacity);
    if (!body) { rc = -5; goto fail; }

    for (;;) {
        if (used + READ_CHUNK_SIZE + 1 > capacity) {
            size_t next = capacity * 2;
            if (next < used + READ_CHUNK_SIZE + 1) next = used + READ_CHUNK_SIZE + 1;
            if (next > MAX_RESPONSE_SIZE + 1) next = MAX_RESPONSE_SIZE + 1;
            if (next <= capacity) {
                if (error_text && error_text_size) snprintf(error_text, error_text_size, "API response was too large");
                rc = -6;
                goto fail;
            }
            char *grown = (char *)realloc(body, next);
            if (!grown) { rc = -7; goto fail; }
            body = grown;
            capacity = next;
        }

        int got = sceHttpReadData(req, body + used, READ_CHUNK_SIZE);
        if (got < 0) { rc = got; goto fail; }
        if (got == 0) break;
        used += (size_t)got;
        if (used > MAX_RESPONSE_SIZE) {
            if (error_text && error_text_size) snprintf(error_text, error_text_size, "API response was too large");
            rc = -8;
            goto fail;
        }
    }

    body[used] = '\0';
    *response_body = body;
    *response_size = used;

    sceHttpDeleteRequest(req);
    sceHttpDeleteConnection(conn);
    sceHttpDeleteTemplate(tpl);
    return 0;

fail:
    if (error_text && error_text_size && error_text[0] == '\0') {
        snprintf(error_text, error_text_size, "HTTP request failed: 0x%08X", rc);
    }
    free(body);
    if (req >= 0) {
        sceHttpAbortRequest(req);
        sceHttpDeleteRequest(req);
    }
    if (conn >= 0) sceHttpDeleteConnection(conn);
    if (tpl >= 0) sceHttpDeleteTemplate(tpl);
    return rc;
}

void vfr_http_free(char *response_body) {
    free(response_body);
}
