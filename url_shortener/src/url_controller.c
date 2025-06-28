#include "url_controller.h"
#include "logger.h"
#include "redis_store.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern char g_base_url[256];
extern int g_url_ttl;

enum MHD_Result handle_shorten(struct MHD_Connection *connection, const char *url, const char *upload_data, size_t *upload_data_size, void **con_cls) {
    (void) url;

    static char *post_data = NULL;

    if (*con_cls == NULL) {
        *con_cls = (void*)1;
        return MHD_YES;
    }

    if (*upload_data_size != 0) {
        post_data = strndup(upload_data, *upload_data_size);
        *upload_data_size = 0;
        return MHD_YES;
    }

    if (post_data) {
        char short_id[7];
        const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        for (int i = 0; i < 6; ++i) {
            short_id[i] = charset[rand() % (sizeof(charset) - 1)];
        }
        short_id[6] = '\0';

        if (redis_save_url(short_id, post_data, g_url_ttl) != 0) {
            free(post_data);
            return MHD_NO;
        }

        char response_json[512];
        snprintf(response_json, sizeof(response_json), "{\"short_url\": \"%s%s\"}\n", g_base_url, short_id);

        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(response_json), response_json, MHD_RESPMEM_MUST_COPY);
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);

        free(post_data);
        return ret;
    }

    return MHD_NO;
}

enum MHD_Result handle_redirect(struct MHD_Connection *connection, const char *short_id) {
    char *original_url = redis_get_url(short_id);
    if (original_url) {
        struct MHD_Response *response = MHD_create_response_from_buffer(0, "", MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(response, "Location", original_url);
        int ret = MHD_queue_response(connection, MHD_HTTP_FOUND, response);
        MHD_destroy_response(response);
        free(original_url);
        return ret;
    }

    const char *msg = "Not Found\n";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(msg), (void *)msg, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response(response);
    return ret;
}
