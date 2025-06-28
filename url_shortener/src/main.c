#include <stdlib.h>
#include <stdio.h>
#include "logger.h"
#include "redis_store.h"
#include "server.h"

int main() {
    logger_init("app.log");

    const char *redis_host = getenv("REDIS_HOST");
    if (!redis_host) redis_host = "127.0.0.1";

    const char *redis_port_str = getenv("REDIS_PORT");
    int redis_port = redis_port_str ? atoi(redis_port_str) : 6379;

    const char *listen_port_str = getenv("APP_PORT");
    int app_port = listen_port_str ? atoi(listen_port_str) : 8888;

    const char *base_url = getenv("BASE_URL");
    if (!base_url) base_url = "http://localhost:8888/";

    const char *ttl_str = getenv("URL_TTL");
    int url_ttl = ttl_str ? atoi(ttl_str) : (30 * 24 * 60 * 60);

    log_message(6, "Starting with Redis %s:%d, listening on port %d, TTL %d seconds, base URL %s",
                redis_host, redis_port, app_port, url_ttl, base_url);

    if (redis_connect(redis_host, redis_port) != 0) {
        log_message(3, "Failed to connect to Redis at %s:%d", redis_host, redis_port);
        return 1;
    }

    set_server_config(app_port, base_url, url_ttl);

    if (start_http_server(app_port) != 0) {
        log_message(3, "Failed to start HTTP server");
        redis_disconnect();
        return 1;
    }

    redis_disconnect();
    logger_close();
    return 0;
}
