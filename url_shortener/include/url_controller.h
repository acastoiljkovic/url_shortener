#ifndef URL_CONTROLLER_H
#define URL_CONTROLLER_H

#include <microhttpd.h>

enum MHD_Result handle_shorten(struct MHD_Connection *connection, const char *url, const char *upload_data, size_t *upload_data_size, void **con_cls);
enum MHD_Result handle_redirect(struct MHD_Connection *connection, const char *short_id);

#endif
