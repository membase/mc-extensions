/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <syslog.h>
#include <stdarg.h>

#include "syslog_extension.h"

static const char *get_name(void) {
    return "syslog";
}

static void logit(EXTENSION_LOG_LEVEL severity,
                  const void* client_cookie,
                  const char *fmt, ...)
{
    (void)client_cookie;

    va_list ap;
    va_start(ap, fmt);
    int sev = LOG_INFO;

    switch (severity) {
    case EXTENSION_LOG_INFO:
        sev = LOG_INFO;
        break;
    case EXTENSION_LOG_WARNING:
        sev = LOG_WARNING;
        break;
    case EXTENSION_LOG_DETAIL:
    case EXTENSION_LOG_DEBUG:
    default:
        sev = LOG_DEBUG;
    }
#ifdef HAVE_VSYSLOG
    vsyslog(sev, fmt, ap);
#else
    char buffer[1024];
    if (vsnprintf(buffer, sizeof(buffer), fmt, ap) != -1) {
        syslog(sev, "%s", buffer);
    }
#endif
    va_end(ap);
}

static EXTENSION_LOGGER_DESCRIPTOR descriptor = {
    .get_name = get_name,
    .log = logit
};

static void exithandler(void) {
    closelog();
}

#if defined (__SUNPRO_C) && (__SUNPRO_C >= 0x550)
__global
#elif defined __GNUC__
__attribute__ ((visibility("default")))
#endif
EXTENSION_ERROR_CODE memcached_extensions_initialize(const char *config,
                                                     GET_SERVER_API get_server_api) {
    (void)config;

    SERVER_HANDLE_V1 *server = get_server_api();
    if (server == NULL || server->interface != 1) {
        return EXTENSION_FATAL;
    }

    if (!server->extension->register_extension(EXTENSION_LOGGER, &descriptor)) {
        return EXTENSION_FATAL;
    }

    openlog("membase",  LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);
    atexit(exithandler);

    return EXTENSION_SUCCESS;
}
