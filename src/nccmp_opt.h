/*
   Copyright (C) 2004,2009 Remik Ziemlinski <surname at fastmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.
   If not, write to the Free Software Foundation,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

/*
        Option & argument parsing from command-line.        
*/
#ifndef NCCMP_OPT_H
#define NCCMP_OPT_H 1

#include "nccmp_common.h"

#ifdef HAVE_GETOPT_H
#  include <getopt.h>
#else
#  include "getopt.h"
#endif

#include "nccmp_strlist.h"
#include <netcdf.h>
#include <nccmp_constants.h>

/* macro is defined at compile time from within Makefile, not config.h */
#define USAGE "\
Compare two NetCDF files.\n\
nccmp is silent when files are equivalent, otherwise it will echo to STDERR whether metadata or a specific variable differs.  By default, comparing stops after the first difference.\n\
\n\
Exit code 0 is returned for identical files, 1 for different files, and 2 for a fatal error.\n\
\n\
Usage: nccmp [OPTION...] file1 [file2]\n\
\n\
  -A, --Attribute=att1[,...] Ignore attribute(s) att1[,...]\n\
                             for all variables.\n\
  -b, --verbose              Print messages to show progress.\n\
  -B, --buffer-entire-var    Load entire variable into memory\n\
                             to reduce memory allocation runtime overhead.\n\
  -c, --var-diff-count=COUNT Stop comparing a variable after COUNT data diffs.\n\
  -C, --diff-count=COUNT     Stop comparing after COUNT data diffs total.\n\
  -d, --data                 Compare data (variable values).\n\
  -D, --debug                Prints verbose debug messages.\n\
  -e, --encoding             Compare these variable encoding settings:\n\
                             checksum, chunk, deflate, endian, shuffle, szip\n\
  -f, --force                Forcefully compare, do not stop after first\n\
                             difference.\n\
  -F, --fortran              Print position indices using Fortran style\n\
                             (1-based reverse order).\n\
  -g, --global               Compare global attributes.\n\
                             (auto enables -m)\n\
  -G, --globalex att1[,...]  Exclude global attributes att1[,...].\n\
                             (auto enables -g)\n\
  -h, --history              Compare global history attribute.\n\
                             (auto enables -g)\n\
  -H, --help                 Give this usage message.\n\
      --usage                \n\
  -i, --info                 Print file structure info for one or both files.\n\
  -l, --color                Print colored messages.\n\
  -m, --metadata             Compare metadata.\n\
                             Use -g to include global attributes.\n\
  -M, --missing              Ignore difference between values that have\n\
                             different missing_value and/or _FillValue.\n\
                             Attribute differences are still reported.\n\
  -n, --threads=N            Use N POSIX threads. Default is 1.\n\
  -N, --nans-are-equal       Allow NaNs to be equal.\n\
  -p, --precision=FMT        Global precision of difference printing.\n\
                             Use '%%x' to print bytes.\n\
                             FMT defaults differently per data type:\n\
                                 double '%%g'\n\
                                 float  '%%g'\n\
                                 int    '%%d'\n\
                                 int64  '%%lld'\n\
                                 schar  '0x%%02X'\n\
                                 short  '%%d'\n\
                                 text   '%%c'\n\
                                 uchar  '0x%%02X'\n\
                                 uint   '%%d'\n\
                                 uint64 '%%llu'\n\
                                 ushort '%%d'\n\
  -P, --header-pad           Compare header padding in classic or 64bit\n\
                             file formats.\n\
  -q, --quiet                Do not print metadata or data differences.\n\
  -r, --group=group1[,...]   Compare group(s) group1[,...] only.\n\
                             Group names can be full or short.\n\
  -s, --report-identical-files\n\
                             Report when files are the same.\n\
  -S, --statistics           Report data difference statistics.\n\
  -t, --tolerance=TOL        Compare if absolute difference > TOL.\n\
  -T, --Tolerance=TOL        Compare if relative percent difference > TOL.\n\
  -v, --variable=var1[,...]  Compare variable(s) var1[,...] only.\n\
  -V, --version              Print program version.\n\
  -x, --exclude=var1[,...]   Exclude variable(s) var1[,...].\n\
  -w, --warn=tag[,...]       Selectively make certain differences\n\
                             exceptions, but still print messages.\n\
                             Supported tags and their meanings:\n\
                                 all: All differences.\n\
                                 format: File formats.\n\
                                 eos: Extra attribute end-of-string nulls.\n\
\n\
Report bugs to https://gitlab.com/remikz/nccmp\n\
"

typedef struct NCCMPOPTS
{
    char    abstolerance;          /* flag, 0=percentage (0-100+), 1=absolute tolerance */
    char    buffer_entire_var;     /* Load entire variable into memory if not zero. */
    char    color;                 /* Print color messages if non-zero. */
    int     data;                  /* compare variable values? */
    int     debug;                 /* print additional debug messages */
    int     diff_count;            /* stop printing differences after count for all vars. */
    int     encoding;              /* do variable encoding comparisons */
    int     exclude;               /* exclude list given */
    nccmp_strlist_t* excludeattlist; /* variable attributes to exclude from comparisons */
    nccmp_strlist_t* excludelist;  /* variables to exclude from comparison */
    char*   file1;                 /* two files to compare */
    char*   file2;                 /*                      */
    int     force;                 /* continue checking after 1st difference */
    int     is_fortran;               /* fortran style index printing */
    int     global;                /* compare global attributes */
    nccmp_strlist_t* globalexclude;/* list of global atts to exclude */
    nccmp_strlist_t* groupnames;   /* list of group names to compare */
    char    header_pad;            /* Compare header padding in classic/64 formats. */
    int     help;                  /* give usage insructions */
    int     history;               /* compare global history attribute */
    char    info;                  /* flag, to print file structure info */
    int     metadata;              /* compare metadata, excluding global attributes */
    int     missing;               /* ignore different missing_value/_FillValues */
    char    nanequal;              /* Nans should be considered equal if compared. */
    char*   precision;             /* print precision of values. */
    char    quiet;                 /* If enabled, do not print diffs. */
    char    report_identical;      /* Print message if files match. */
    char    statistics;            /* If should enable diff statistics gathering. */
    int     threads;               /* Number of threads to use. Must be 1 or greater. */
    double  tolerance;             /* allowable difference tolerance */
    int     var_diff_count;        /* stop printing differences after count for each var. */
    int     variable;              /* variable list given */
    int     verbose;               /* print messages to show progress */
    int     version;               /* give program version */
    nccmp_strlist_t*  variablelist; /* specific variables to compare */
char    warn[NCCMP_WARN_NUM_TAGS]; /* booleans if a warning is on/off */
} nccmp_opt_t;

int nccmp_opt_parse(int argc, const char** argv, nccmp_opt_t* popts);

void print_usage();

void print_version();

/* frees pointers in struct */
void nccmp_opt_free(nccmp_opt_t* popts);

void nccmp_opt_init(nccmp_opt_t* popts);

#endif /* NCCMP_OPT_H */
