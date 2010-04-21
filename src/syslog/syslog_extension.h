#ifndef SYSLOG_EXTENSION_H
#define SYSLOG_EXTENSION_H

#include "memcached/engine.h"

/* prototype required to avoid warnings treated as failures on some *NIX */
EXTENSION_ERROR_CODE memcached_extensions_initialize(const char *config,
                                                     GET_SERVER_API get_server_api);
#endif
