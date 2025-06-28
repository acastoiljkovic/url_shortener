#ifndef HEALTH_CONTROLLER_H
#define HEALTH_CONTROLLER_H

#include <microhttpd.h>
#include "redis_store.h"

enum MHD_Result handle_liveness(struct MHD_Connection *connection);
enum MHD_Result handle_readiness(struct MHD_Connection *connection);
enum MHD_Result handle_version(struct MHD_Connection *connection);

#endif
