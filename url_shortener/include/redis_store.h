#ifndef REDIS_STORE_H
#define REDIS_STORE_H

#include <hiredis/hiredis.h>

extern redisContext *redis;

int redis_connect(const char *hostname, int port);
int redis_save_url(const char *short_id, const char *url, int ttl_seconds);
char *redis_get_url(const char *short_id);
void redis_disconnect(void);

#endif
