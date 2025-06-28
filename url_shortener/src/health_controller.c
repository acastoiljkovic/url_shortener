#include "health_controller.h"
#include "logger.h"
#include "redis_store.h"
#include <string.h>
#include <stdio.h>
#include <hiredis/hiredis.h>
#include "version.h"

extern redisContext *redis;

enum MHD_Result handle_liveness(struct MHD_Connection *connection) {
    const char *msg = "OK - service is alive\n";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(msg), (void *)msg, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    log_message(6, "Liveness check requested, returning 200 OK");
    return ret;
}

enum MHD_Result handle_readiness(struct MHD_Connection *connection) {
    redisReply *reply = redisCommand(redis, "PING");
    if (reply && reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "PONG") == 0) {
        const char *msg = "OK - service is ready\n";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(msg), (void *)msg, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        freeReplyObject(reply);
        log_message(6, "Readiness check passed, returning 200 OK");
        return ret;
    }
    if (reply) freeReplyObject(reply);
    const char *msg = "Service not ready - Redis unavailable\n";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(msg), (void *)msg, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_SERVICE_UNAVAILABLE, response);
    MHD_destroy_response(response);
    log_message(4, "Readiness check failed, Redis unavailable");
    return ret;
}

enum MHD_Result handle_version(struct MHD_Connection *connection) {
    char response[64];
    snprintf(response, sizeof(response), "Version: %s\n", APP_VERSION);

    struct MHD_Response *res = MHD_create_response_from_buffer(strlen(response), (void*)response, MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, res);
    MHD_destroy_response(res);

    log_message(6, "Version check requested, returning %s", APP_VERSION);
    return ret;
}
