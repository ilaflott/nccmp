/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* When built. */
#define BUILD_DATE "Sun Sep 27 20:47:02 EDT 2015"

/* Where built. */
#define BUILD_HOST "Linux mint 3.16.0-38-generic #52~14.04.1-Ubuntu SMP Fri May 8 09:43:57 UTC 2015 x86_64 x86_64 x86_64 GNU/Linux"

/* Who built this. */
#define BUILD_USER "rsz"

/* Flags passed to configure. */
#define CONFIG_CC "gcc"

/* Flags passed to configure. */
#define CONFIG_CFLAGS "-I/usr/local/hdf5-1.8.15-patch1/include -I/usr/local/netcdf-4.3.3.1/include -pthread"

/* Flags passed to configure. */
#define CONFIG_CPP "gcc -E"

/* Flags passed to configure. */
#define CONFIG_CPPFLAGS ""

/* Flags passed after ./configure. */
#define CONFIG_FLAGS ""

/* Flags passed to configure. */
#define CONFIG_LDFLAGS "-L/usr/local/hdf5-1.8.15-patch1/lib -L/usr/local/netcdf-4.3.3.1/lib -pthread"

/* Flags passed to configure. */
#define CONFIG_LIBS "-lnetcdf -lm "

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `m' library (-lm). */
#define HAVE_LIBM 1

/* Define to 1 if you have the `netcdf' library (-lnetcdf). */
#define HAVE_LIBNETCDF 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if netcdf api sets the first usertype id. */
#define HAVE_NC_FIRSTUSERTYPEID 1

/* Define to 1 if you have <nc.h> and <nc3internal.h>. */
/* #undef HAVE_NC_H */

/* Set to 1 if netcdf library has nc_inq_path. */
#define HAVE_NC_INQ_PATH 1

/* Define to 1 if you have the <netcdf.h> header file. */
#define HAVE_NETCDF_H 1

/* Define to 1 if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Have PTHREAD_PRIO_INHERIT. */
#define HAVE_PTHREAD_PRIO_INHERIT 1

/* Define to 1 if you have the <stdbool.h> header file. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if the system has the type `_Bool'. */
#define HAVE__BOOL 1

/* Name of package */
#define PACKAGE "nccmp"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "Remik.Ziemlinski at noaa.gov"

/* Define to the full name of this package. */
#define PACKAGE_NAME "nccmp"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "nccmp 1.7.4.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "nccmp"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.7.4.0"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "1.7.4.0"

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */
