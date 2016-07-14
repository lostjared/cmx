INCLUDES=$(all_includes)
AM_CFLAGS=-I$(HEADER_PATH) $(PLATFORM_CFLAGS) 
AM_LDFLAGS=$(FINAL_LDFLAGS)
# the library search path.
#include_HEADERS=
lib_LTLIBRARIES=libcmx.la

libcmx_la_CPPFLAGS= -I$(HEADER_PATH) $(PLATFORM_CFLAGS) -I$(_PREFIX)/include
libcmx_la_SOURCES=system.cc thread.cc video.cc event.cc types.cc font.cc debug.cc parser.cc joystick.cc fbrowse.cc gl.cc
libcmx_la_LDFLAGS= $(PLATFORM_LDFLAGS) -version-info 1:0:0
libcmx_includedir=$(includedir)/cmx
libcmx_include_HEADERS=../cmx/include/*.h

