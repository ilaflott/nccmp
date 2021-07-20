dnl Process this file with `autoreconf -i` to produce a configure script.
AC_INIT(nccmp, 1.9.0.1, [[https://gitlab.com/remikz/nccmp]])

dnl Include additional scripts.
AC_CONFIG_MACRO_DIR([m4])

dnl Capture flags after ./configure to embed into version message.
config_flags="$*"
 
AM_INIT_AUTOMAKE

AC_LANG([C])

AC_CONFIG_HEADERS(config.h)

dnl Check for netcdf source includes to use private functions not in library.
AC_ARG_WITH( [netcdf],
             [ AS_HELP_STRING([--with-netcdf=dir],
                [Path to full NetCDF source package. Enables header-pad comparison.])],
             [ with_netcdf="$withval"
               AC_DEFINE(HAVE_NC_H,1,[Define to 1 if you have <nc.h> and <nc3internal.h>.])
               HAVE_NC_H="yes"
               AM_CFLAGS="$AM_CFLAGS -I$with_netcdf/include -I$with_netcdf/libsrc4"
             ],
             [ AC_MSG_WARN([[NetCDF source package not set with --with-netcdf. Download from www.unidata.ucar.edu]])
               HAVE_NC_H="no"
             ]
            )

dnl For aggressive compiler checks during development.
dnl GCC
dnl AM_CFLAGS="$AM_CFLAGS -std=c99 -Wpedantic -Wall -Wclobbered -Wempty-body -Wignored-qualifiers -Wmissing-field-initializers -Wmissing-parameter-type -Wold-style-declaration -Woverride-init -Wtype-limits -Wuninitialized -Wunused-but-set-parameter -D_BSD_SOURCE"
dnl Clang
dnl AM_CFLAGS="$AM_CFLAGS -std=c99 -Wpedantic -Wall -Wconsumed -Wempty-body -Wignored-qualifiers -Wmissing-field-initializers -Wout-of-line-declaration -Wtype-limits -Wuninitialized -Wunused-variable -D_BSD_SOURCE"

AC_SUBST([AM_CFLAGS])

dnl Check if big file test should be run.
AC_ARG_ENABLE( [test-big],
               AS_HELP_STRING([--enable-test-big], [Enable big file (multi-gigabyte) test.])
             )
dnl Make explicit "no".
if test "$enable_test_big" != yes; then
    enable_test_big="no"
fi
dnl Symbol to be used in test/Makefile.am.
AM_CONDITIONAL([ENABLE_TEST_BIG], [test x$enable_test_big = xyes])

dnl Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

dnl Checks for libraries.
AC_CHECK_LIB(m, floor) 
AC_CHECK_LIB(netcdf, nc_open, 
		     [],
		     AC_MSG_FAILURE([[Required NetCDF library not found!]]))
AC_CHECK_FUNC(nc_inq_path, 
              [ AC_DEFINE(HAVE_NC_INQ_PATH,1,[Set to 1 if netcdf library has nc_inq_path.]) ],
              AC_MSG_WARN([[Netcdf library does not have nc_inq_path.]]))
if test "$ac_cv_func_nc_inq_path" = yes; then
    HAVE_NC_INQ_PATH="yes"
else
    HAVE_NC_INQ_PATH="no"
fi
dnl Check if netcdf has first user-type id.
AC_CHECK_DECL(  [NC_FIRSTUSERTYPEID],
                [ AC_DEFINE(HAVE_NC_FIRSTUSERTYPEID, 1, [Define to 1 if netcdf api sets the first usertype id.])],
                [],
                [#include <netcdf.h>])

dnl Checks for header files.
AC_HEADER_STDBOOL
AC_HEADER_STDC
AC_CHECK_HEADERS(getopt.h errno.h fcntl.h limits.h memory.h netcdf.h strings.h string.h stdbool.h sys/stat.h sys/types.h unistd.h)

AM_CONDITIONAL([SYSTEM_GETOPT], [test x$ac_cv_header_getopt_h = xyes])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_SYS_LARGEFILE

AX_PTHREAD([ AC_DEFINE(HAVE_PTHREAD,1,[Define to 1 if you have POSIX threads libraries and header files.])
             CLIBS="$PTHREAD_LIBS $LIBS"
             CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
             LDFLAGS="$LDFLAGS $PTHREAD_CFLAGS"
             HAVE_PTHREAD="yes" ],
           [ AC_MSG_WARN([[Pthread library not found. Please set PTHREAD_CFLAGS and PTHREAD_LIBS.]])
             HAVE_PTHREAD="no" 
           ]
           )

AC_DEFINE_UNQUOTED([CONFIG_CC],      ["$CC"],          [Flags passed to configure.])
AC_DEFINE_UNQUOTED([CONFIG_CFLAGS],  ["$CFLAGS"],      [Flags passed to configure.])
AC_DEFINE_UNQUOTED([CONFIG_CPP],     ["$CPP"],         [Flags passed to configure.])
AC_DEFINE_UNQUOTED([CONFIG_CPPFLAGS],["$CPPLAGS"],     [Flags passed to configure.])
AC_DEFINE_UNQUOTED([CONFIG_FLAGS],   ["$config_flags"],[Flags passed after ./configure.])
AC_DEFINE_UNQUOTED([CONFIG_LDFLAGS], ["$LDFLAGS"],     [Flags passed to configure.])
AC_DEFINE_UNQUOTED([CONFIG_LIBS],    ["$LIBS"],        [Flags passed to configure.])
AC_DEFINE_UNQUOTED([BUILD_USER],     ["$USER"],        [Who built this.])
AC_DEFINE_UNQUOTED([BUILD_DATE],     ["`date`"],       [When built.])
AC_DEFINE_UNQUOTED([BUILD_HOST],     ["`uname -a`"],   [Where built.])
 
AC_OUTPUT(Makefile man/Makefile src/Makefile test/Makefile)

AC_MSG_NOTICE([

    CFLAGS             = $CFLAGS
    CPPFLAGS           = $CPPFLAGS
    enable-test-big    = $enable_test_big
    HAVE_NC_H          = $HAVE_NC_H
    HAVE_NC_INQ_PATH   = $HAVE_NC_INQ_PATH
    HAVE_PTHREAD       = $HAVE_PTHREAD
    LDFLAGS            = $LDFLAGS 
    LIBS               = $LIBS        
    with-netcdf        = $with_netcdf
])
