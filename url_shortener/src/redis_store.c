#include <hiredis/hiredis.h>
#include <stdlib.h>
#include <string.h>
#include "redis_store.h"
#include "logger.h"

redisContext *redis = NULL;

int redis_connect(const char *hostname, int port) {
    redis = redisConnect(hostname, port);
    return (redis && !redis->err) ? 0 : -1;
}

void redis_disconnect() {
    if (redis) redisFree(redis);
}

int redis_save_url(const char *short_id, const char *url, int ttl_seconds) {
    if (!redis) return -1;
    redisReply *reply = redisCommand(redis, "SETEX %s %d %s", short_id, ttl_seconds, url);
    if (!reply) return -1;
    freeReplyObject(reply);
    return 0;
}

char *redis_get_url(const char *short_id) {
    if (!redis) return NULL;
    redisReply *reply = redisCommand(redis, "GET %s", short_id);
    if (!reply) return NULL;
    char *result = NULL;
    if (reply->type == REDIS_REPLY_STRING) {
        result = strdup(reply->str);
    }
    freeReplyObject(reply);
    return result;
}
