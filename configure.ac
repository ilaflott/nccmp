dnl Process this file with autoconf to produce a configure script.
AC_INIT(nccmp, 1.5.0, [[Remik.Ziemlinski_at_noaa.gov]]) 
AM_INIT_AUTOMAKE

AC_LANG([C])

AC_CONFIG_HEADERS(config.h)

AC_ARG_ENABLE([ncinfo],
  [AS_HELP_STRING([--enable-ncinfo=yes|no],
    [Build and install helper tool that prints file structure info. Default is yes.])],
  [enable_ncinfo="$enableval"],
  [enable_ncinfo="yes"])
AM_CONDITIONAL([ENABLE_NCINFO], [test x$enable_ncinfo = xyes])

dnl Check for netcdf source includes to use private functions not in library.
AC_ARG_WITH([netcdf],
  [AS_HELP_STRING([--with-netcdf=prefix],
    [Path to full NetCDF source package. Required.])],
  [with_netcdf="$withval"],
  AC_MSG_ERROR([[Required NetCDF source package not set with --with-netcdf. Download from www.unidata.ucar.edu]]))

AM_CFLAGS="$AM_CFLAGS -I$with_netcdf/include -I$with_netcdf/libsrc4"
AC_SUBST([AM_CFLAGS])

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

dnl Checks for header files.
AC_HEADER_STDBOOL
AC_HEADER_STDC
AC_CHECK_HEADERS(netcdf.h fcntl.h getopt.h limits.h strings.h unistd.h string.h sys/types.h sys/stat.h errno.h memory.h stdbool.h)

AM_CONDITIONAL([SYSTEM_GETOPT], [test x$ac_cv_header_getopt_h = xyes])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_SYS_LARGEFILE

dnl Checks for library functions. 
AC_OUTPUT(src/Makefile test/Makefile Makefile ncinfo/Makefile)

AC_MSG_NOTICE([

    CFLAGS   = $CFLAGS
    CPPFLAGS = $CPPFLAGS
    LDFLAGS  = $LDFLAGS 
    LIBS     = $LIBS 
    
    with-netcdf = $with_netcdf
])