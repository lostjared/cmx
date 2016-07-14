bin_PROGRAMS=afhd afhd_gl afhd3d

afhd3d_SOURCES=afhd3d.cc fixedsys.bin.c
afhd3d_CPPFLAGS=-I$(HEADER_PATH) $(PLATFORM_CFLAGS)
afhd3d_LDFLAGS= -L../$(PLATFORM) -lcmx $(PLATFORM_LDFLAGS)
afhd_SOURCES=afhd.cc main.cc fixedsys.bin.c
afhd_CPPFLAGS=-I$(HEADER_PATH) $(PLATFORM_CFLAGS) 
afhd_LDFLAGS=-L../$(PLATFORM) -lcmx $(PLATFORM_LDFLAGS) 
afhd_gl_SOURCES=afhd_gl.cc fixedsys.bin.c
afhd_gl_CPPFLAGS=-I$(HEADER_PATH) $(PLATFORM_CFLAGS)
afhd_gl_LDFLAGS=-L../$(PLATFORM) -lcmx $(PLATFORM_LDFLAGS)
