AC_PREREQ([2.64])
AC_INIT(libcrossmx, 1.0, jared@lostsidedead.biz)
AM_INIT_AUTOMAKE


# Checks for programs.
AC_CONFIG_HEADERS(config.h)
AC_PROG_CXX
AC_PROG_CC
AC_LANG_CPLUSPLUS
#AC_PROG_RANLIB
AM_PROG_LIBTOOL
# Checks for libraries.
# Checks for header files.
# Checks for typedefs, structures, and compiler characteristics.
# Checks for library functions.
LDPREFIX=${prefix}
if test  ${prefix} = "NONE"; then
LDPREFIX=/usr/local
fi
_PREFIX=$LDPREFIX
AC_ARG_VAR(platform, Determines platform)
CUR_OS="$host"
TEST_STR=$CUR_OS
OPENGL_FLAGS="-lGL -lGLU"
CFLAG=""
case $TEST_STR in
*apple*)
	CUR_OS="apple darwin"
	OPENGL_FLAGS="-framework OpenGL"
	CFLAG="-O2 -g "
	;;
*mingw*)
	CUR_OS="Windows"
	OPENGL_FLAGS="-lopengl32 -lglu32 "
	if test $platform = "windows"; then
	CFLAG="-O2 -g"
	else
	CFLAG="-Dmain=SDL_main"
	fi
	;;
esac

echo $CUR_OS
if test x$platform = "x"; then
PLATFORM="generic"

PLATFORM_CFLAGS="$CFLAG  `$_PREFIX/bin/libpng-config --cflags` `$_PREFIX/bin/sdl-config --cflags` -I$_PREFIX/include "
PLATFORM_LDFLAGS="$CFLAG `$_PREFIX/bin/sdl-config --libs` -L$_PREFIX/lib  -ljpeg -lz $OPENGL_FLAGS `$_PREFIX/bin/libpng-config --libs --static`" 

AC_DEFINE_UNQUOTED(PLATFORM_GENERIC, 1, [platform generic])
elif test x$platform = "xgeneric"; then
#generic SDL implementation
PLATFORM="generic"
PLATFORM_CFLAGS="$CFLAG  `$_PREFIX/bin/libpng-config --cflags` `$_PREFIX/bin/sdl-config --cflags` -I$_PREFIX/include "
PLATFORM_LDFLAGS="$CFLAG `$_PREFIX/bin/sdl-config --libs` -L$_PREFIX/lib  -ljpeg -lz $OPENGL_FLAGS `$_PREFIX/bin/libpng-config --libs --static`  "
AC_DEFINE_UNQUOTED(PLATFORM_GENERIC, 1, [platform generic])
elif test x$platform = "xosx"; then
PLATFORM="osx"
PLATFORM_CFLAGS=""
PLATFORM_LDFLAGS=""

#setup stuff for osx
elif test x$platform = "xwindows"; then
PLATFORM="windows"
# setup path's etc for win
PLATFORM_CFLAGS="`$_PREFIX/bin/libpng-config --cflags` -I$_PREFIX/include -O2 -g"
PLATFORM_LDFLAGS="`$_PREFIX/bin/linbpng-config --static --libs` -L$_PREFIX/lib -lmingw32 -mwindows -lm -luser32 -lgdi32 -lwinmm -ldinput8 -ldxguid -lopengl32 -lglu32 -lpng -lz"
AC_DEFINE_UNQUOTED(PLATFORM_WINDOWS, 1, [platform generic])
else
AC_MSG_ERROR([ Unsupported project platform: $platform ])
fi
HEADER_PATH=`pwd`/cmx/include
echo "header path: $HEADER_PATH"
echo "project platform: $PLATFORM"
echo "prefix: $_PREFIX"
echo "CPPFLAGS: $PLATFORM_CFLAGS"
echo "LDFLAGS: $PLATFORM_LDFLAGS"

if test $PLATFORM = "generic"; then
if test -f $_PREFIX/bin/sdl-config; then
echo "sdl-config found in prefix: $_PREFIX/bin/sdl-config"
else
AC_MSG_ERROR([ Could not find sdl-config use --prefix on configure ])
fi
fi

APP_PLATFORM_LDFLAGS="$CFLAG -L../$PLATFORM -lcmx  `$_PREFIX/bin/sdl-config --static-libs` -L$_PREFIX/lib -ljpeg -lz $OPENGL_FLAGS `$_PREFIX/bin/libpng-config --libs --static`" 


#AC_DEFINE_UNQUOTED(, 1, [debug mode])
AC_SUBST(_PREFIX)
AC_SUBST(PLATFORM)
AC_SUBST(PLATFORM_CFLAGS)
AC_SUBST(PLATFORM_LDFLAGS)
AC_SUBST(FINAL_CFLAGS)
AC_SUBST(FINAL_LDFLAGS)
AC_SUBST(HEADER_PATH)
AC_SUBST(APP_PLATFORM_LDFLAGS)
AC_OUTPUT(Makefile generic/Makefile osx/Makefile windows/Makefile test/Makefile maptest/Makefile filebrowser/Makefile afhd/Makefile gltest/Makefile helloworld/Makefile threadtest/Makefile)

