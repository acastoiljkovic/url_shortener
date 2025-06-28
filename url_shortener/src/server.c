#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include "server.h"
#include "logger.h"
#include "redis_store.h"
#include "health_controller.h"
#include "url_controller.h"

#define MAX_BASE_URL_LEN 256

static struct MHD_Daemon *http_daemon = NULL;
static volatile int running = 1;

int g_url_ttl = 2592000;
char g_base_url[MAX_BASE_URL_LEN] = "http://localhost:8888/";

static void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

static enum MHD_Result handle_request(void *cls, struct MHD_Connection *connection,
                                      const char *url, const char *method,
                                      const char *version, const char *upload_data,
                                      size_t *upload_data_size, void **con_cls) {
    (void) cls;
    (void) version;

    if (strcmp(method, "GET") == 0 && strcmp(url, "/liveness") == 0) {
        return handle_liveness(connection);
    }

    if (strcmp(method, "GET") == 0 && strcmp(url, "/readiness") == 0) {
        return handle_readiness(connection);
    }

    if (strcmp(method, "POST") == 0 && strcmp(url, "/shorten") == 0) {
        return handle_shorten(connection, url, upload_data, upload_data_size, con_cls);
    }

    if (strcmp(method, "GET") == 0 && strcmp(url, "/version") == 0) {
        return handle_version(connection);
    }

    if (strcmp(method, "GET") == 0 && strlen(url) > 1) {
        const char *short_id = url + 1;
        return handle_redirect(connection, short_id);
    }

    const char *msg = "Invalid endpoint\n";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(msg), (void *)msg, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response(response);
    return ret;
}

void set_server_config(int port, const char *base_url, int ttl) {
    (void)port;
    g_url_ttl = ttl;
    snprintf(g_base_url, MAX_BASE_URL_LEN, "%s", base_url);
}

int start_http_server(int port) {
    srand(time(NULL));

    http_daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_SUSPEND_RESUME | MHD_USE_DEBUG,
                                   port, NULL, NULL,
                                   &handle_request, NULL,
                                   MHD_OPTION_END);

    if (!http_daemon) {
        log_message(3, "Failed to start HTTP server");
        return -1;
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    log_message(6, "HTTP server running on port %d", port);

    while (running) {
        sleep(1);
    }

    log_message(6, "Shutting down HTTP server");
    MHD_stop_daemon(http_daemon);
    return 0;
}
