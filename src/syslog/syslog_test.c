
#include "syslog_extension.c"
#undef NDEBUG
#include <assert.h>
#include <string.h>

void closelog(void)
{
    /* I am not able to test this :( */
}

void openlog(const char *ident, int logopt, int facility)
{
    assert(strcmp(ident, "membase") == 0);
    assert(logopt == (LOG_CONS | LOG_NDELAY | LOG_PID));
    assert(facility == LOG_DAEMON);
}

const char *expected_msg;
int expected_priority;

#ifdef HAVE_VSYSLOG
void vsyslog(int priority, const char *fmt, va_list args)
{
    char buffer[1024];
    assert(vsnprintf(buffer, sizeof(buffer), fmt, args) != -1);
    assert(strcmp(expected_msg, buffer) == 0);
    assert(expected_priority == priority);
}
#else
void syslog(int priority, const char *fmt, ...)
{
    char buffer[1024];
    va_list ap;
    va_start(ap, fmt);
    assert(vsnprintf(buffer, sizeof(buffer), fmt, ap) != -1);
    va_end(ap);

    assert(strcmp(expected_msg, buffer) == 0);
    assert(expected_priority == priority);
}
#endif

static SERVER_HANDLE_V1* null_server_api(void)
{
    return NULL;
}

static SERVER_HANDLE_V1* invalid_server_api(void)
{
    static SERVER_HANDLE_V1 rv = {
        .interface = 9999,
    };

    return &rv;
}

bool should_fail_register = true;
EXTENSION_LOGGER_DESCRIPTOR *descr;

static bool register_extension(extension_type_t type, void *extension)
{
    if (should_fail_register) {
        return false;
    }
    assert(extension != NULL);
    assert(type == EXTENSION_LOGGER);
    assert(extension != NULL);
    descr = extension;

    assert(descr->get_name != NULL);
    assert(descr->log != NULL);
    assert(strcmp("syslog", descr->get_name()) == 0);
    return true;
}

static SERVER_HANDLE_V1* get_server_api(void)
{
    static SERVER_EXTENSION_API extension_api = {
        .register_extension = register_extension
    };

    static SERVER_HANDLE_V1 rv = {
        .interface = 1,
        .extension = &extension_api,
    };

    return &rv;
}


int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* lets test that it can handle a NULL server api */
    assert(memcached_extensions_initialize(NULL, null_server_api) == EXTENSION_FATAL);
    /* And invalid (unknown) server api */
    assert(memcached_extensions_initialize(NULL, invalid_server_api) == EXTENSION_FATAL);

    should_fail_register = true;
    assert(memcached_extensions_initialize(NULL, get_server_api) == EXTENSION_FATAL);

    should_fail_register = false;
    assert(memcached_extensions_initialize(NULL, get_server_api) == EXTENSION_SUCCESS);

    expected_msg = "test a string";
    expected_priority = LOG_DEBUG;
    descr->log(EXTENSION_LOG_DETAIL, NULL, "%s", expected_msg);

    /* @todo feel free to add a bunch more tests... */

    return 0;
}
