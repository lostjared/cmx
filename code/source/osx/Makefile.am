INCLUDES= $(all_includes)

# the library search path.
#include_HEADERS=
lib_LTLIBRARIES=libcmx.la
libcmx_la_CPPFLAGS=-I$(HEADER_PATH) $(PLATFORM_CFLAGS)
libcmx_la_SOURCES=system.cc thread.cc video.cc event.cc types.cc font.cc debug.cc parser.cc joystick.cc
libcmx_la_LDFLAGS=-version-info 1:0:0 $(PLATFORM_LDFLAGS)
libcmx_includedir=$(includedir)/cmx
libcmx_include_HEADERS=../cmx/include/*.h
