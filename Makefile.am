ACLOCAL_AMFLAGS = -I m4 --force

lib_LTLIBRARIES = syslog_extension.la

syslog_extension_la_CXXFLAGS = ${NO_ERROR}
syslog_extension_la_LDFLAGS = -module -dynamic
syslog_extension_la_SOURCES = src/syslog/syslog_extension.h \
                              src/syslog/syslog_extension.c

syslog_extension_test_DEPENDENCIES=src/syslog/syslog_extension.h \
                                   src/syslog/syslog_extension.c
syslog_extension_test_SOURCES=src/syslog/syslog_test.c

check_PROGRAMS=syslog_extension_test
TESTS=${check_PROGRAMS}

test: check-TESTS

