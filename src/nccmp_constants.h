#ifndef NCCMP_CONSTANTS_H
#define NCCMP_CONSTANTS_H

#include <config.h>

#if HAVE_NC_FIRSTUSERTYPEID
    /* Do nothing. */
#else
    #define NC_FIRSTUSERTYPEID (NC_COMPOUND + 1)
#endif

/* Warning tags supported, which are used as array indices. */
#define NCCMP_W_TAGS "all,format,eos"
#define NCCMP_WARN_NUM_TAGS 3
#define NCCMP_W_ALL 0
#define NCCMP_W_FORMAT 1
#define NCCMP_W_EOS 2

typedef enum { COUNT_UNDER=0, COUNT_OVER=1, COUNT_EQUAL=2 } count_limit_type;

#define NCCMP_MAX_DIMS_IN_VAR 8

/* Limitation of netcdf API. */
#define NCCMP_MAX_COMPOUND_FIELD_DIMS 4

/* used in allocating a string list */
#define NCCMP_MAX_STRINGS 256

#define NCCMP_MAX_USER_TYPES 256

/* There is no variable for this. Found limit in documentation.
   Sadly, nc_inq_grps() assumes pre-allocated array for group ids.
   https://www.unidata.ucar.edu/software/netcdf/docs/group__groups.html#details
*/
#define NCCMP_MAX_GROUPS 32767

#endif
