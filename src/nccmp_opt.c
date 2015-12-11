/*
   Copyright (C) 2004-2006,2010,2012 Remik Ziemlinski <first d0t surname att n0aa d0t g0v>

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

#include <nccmp_opt.h>
#include <log.h>

static struct option const long_options[] =
{
    {"Attribute",         required_argument,      0, 'A'},
    {"data",              no_argument,            0, 'd'},
    {"debug",             no_argument,            0, 'D'},
    {"diff-count",        required_argument,      0, 'C'},
    {"encoding",          no_argument,            0, 'e'},
    {"exclude",           required_argument,      0, 'x'},
    {"force",             no_argument,            0, 'f'},
    {"fortran",           no_argument,            0, 'F'},
    {"global",            no_argument,            0, 'g'},
    {"globalex",          required_argument,      0, 'G'},
    {"group",             required_argument,      0, 'r'},
    {"header-pad",        no_argument,            0, 'P'},
    {"history",           no_argument,            0, 'h'},
    {"info",              no_argument,            0, 'i'},
    {"metadata",          no_argument,            0, 'm'},
    {"missing",           no_argument,            0, 'M'},
    {"nans-are-equal",    no_argument,            0, 'N'},
    {"precision",         required_argument,      0, 'p'},
    {"quiet",             no_argument,            0, 'q'},
    {"report-identical-files", no_argument,       0, 's'},
    {"statistics",        no_argument,            0, 'S'},
    {"threads",           required_argument,      0, 'n'},
    {"tolerance",         required_argument,      0, 't'},
    {"Tolerance",         required_argument,      0, 'T'},
    {"var-diff-count",    required_argument,      0, 'c'},
    {"variable",          required_argument,      0, 'v'},
    {"verbose",           no_argument,            0, 'b'},
    {"help",              no_argument,            0, 'H'},
    {"usage",             no_argument,            0, 'H'},
    {"version",           no_argument,            0, 'V'},
    {"warn",              required_argument,      0, 'w'},
    {0,                   0,                      0, 0}
};

void nccmp_opt_free(nccmp_opt_t* popts)
{
    nccmp_free_strlist(& popts->excludeattlist);
    nccmp_free_strlist(& popts->excludelist);
    nccmp_free_strlist(& popts->globalexclude);
    nccmp_free_strlist(& popts->groupnames);
    nccmp_free_strlist(& popts->variablelist);
    XFREE(popts->file1);
    XFREE(popts->file2);
    XFREE(popts->precision);
}

void nccmp_opt_init(nccmp_opt_t* popts)
{
    if (popts == NULL) return;

    popts->abstolerance     = 0;
    popts->data             = 0;
    popts->debug            = 0;
    popts->diff_count       = 0;
    popts->encoding         = 0;
    popts->exclude          = 0;
    popts->excludeattlist   = NULL;
    popts->excludelist      = NULL;
    popts->file1            = NULL;
    popts->file2            = NULL;
    popts->force            = 0;
    popts->fortran          = 0;
    popts->global           = 0;
    popts->globalexclude    = NULL;
    popts->groupnames       = NULL;
    popts->header_pad       = 0;
    popts->help             = 0;
    popts->history          = 0;
    popts->info             = 0;
    popts->metadata         = 0;
    popts->missing          = 0;
    popts->nanequal         = 0;
    popts->precision        = NULL;
    popts->quiet            = 0;
    popts->report_identical = 0;
    popts->statistics       = 0;
    popts->threads          = 1;
    popts->tolerance        = 0;
    popts->version          = 0;
    popts->var_diff_count   = 0;
    popts->variable         = 0;
    popts->variablelist     = NULL;
    popts->verbose          = 0;

    memset(popts->warn, 0, NCCMP_WARN_NUM_TAGS);
}

void print_features()
{
    fprintf(stderr, "\nFeatures\n");

    #ifdef HAVE_NC_H
        fprintf(stderr, "header-pad = yes\n");
    #else
        fprintf(stderr, "header-pad = no\n");
    #endif

    #ifdef HAVE_PTHREAD
        fprintf(stderr, "pthreads   = yes\n");
    #else
        fprintf(stderr, "pthreads   = no\n");
    #endif
}

void print_options(nccmp_opt_t* popts)
{
    LOG_DEBUG("abstolerance = %d\n", popts->abstolerance);
    LOG_DEBUG("data = %d\n", popts->data);
    LOG_DEBUG("diff_count = %d\n", popts->diff_count);
    LOG_DEBUG("encoding = %d\n", popts->encoding);
    LOG_DEBUG("global = %d\n", popts->global);
    LOG_DEBUG("header_pad = %d\n", popts->header_pad);
    LOG_DEBUG("history = %d\n", popts->history);
    LOG_DEBUG("metadata = %d\n", popts->metadata);
    LOG_DEBUG("nanequal = %d\n", popts->nanequal);
    LOG_DEBUG("precision = %s\n", popts->precision);
    LOG_DEBUG("quiet = %d\n", popts->quiet);
    LOG_DEBUG("statistics = %d\n", popts->statistics);
    LOG_DEBUG("threads = %d\n", popts->threads);
    LOG_DEBUG("tolerance = %g\n", popts->tolerance);
    LOG_DEBUG("var_diff_count = %d\n", popts->var_diff_count);
}

void print_copyright()
{
    fprintf(stderr, "\nCopyright (C) 2004-2007,2009,2010,2012-2015 Remik Ziemlinski\n \
\n\
This program comes with NO WARRANTY, to the extent permitted by law.\n\
You may redistribute copies of this program\n\
under the terms of the GNU General Public License.\n");
}

void print_usage()
{
    fprintf(stderr, USAGE);
}

void print_version()
{
    fprintf(stderr, "nccmp %s\n\n", VERSION);
    fprintf(stderr, "Configuration\n");
    fprintf(stderr, "Date    = %s\n", BUILD_DATE);
    fprintf(stderr, "Host    = %s\n", BUILD_HOST);
    fprintf(stderr, "NetCDF  = %s\n", nc_inq_libvers());
    fprintf(stderr, "User    = %s\n", BUILD_USER);
    fprintf(stderr, "Flags   = CC=\"%s\" \\\n"
                    "          CFLAGS=\"%s\" \\\n"
                    "          CPP=\"%s\" \\\n"
                    "          CPPFLAGS=\"%s\" \\\n"
                    "          LDFLAGS=\"%s\" \\\n"
                    "          LIBS=\"%s\" \\\n"
                    "          ./configure %s\n",
                    CONFIG_CC, CONFIG_CFLAGS, CONFIG_CPP, CONFIG_CPPFLAGS,
                    CONFIG_LDFLAGS, CONFIG_LIBS, CONFIG_FLAGS);
}

int nccmp_opt_parse(int argc, const char** argv, nccmp_opt_t* popts)
{
    int c, i, status = EXIT_SUCCESS;
    nccmp_strlist_t* warningtags = NULL;
    nccmp_strlist_t* valid_warnings = NULL;

    /* Create searchable list of defined warning tags. */
    valid_warnings = nccmp_new_strlist(NCCMP_WARN_NUM_TAGS);
    if (!valid_warnings) {
        LOG_ERROR("Failed to allocate memory for warnings list.\n");
        status = EXIT_FATAL;
        goto recover;
    }

    nccmp_split_strlist_by_delim(NCCMP_W_TAGS, valid_warnings, ",");

    if (popts == NULL) {
        LOG_ERROR("Failed to setup warning tags.\n");
        status = EXIT_FATAL;
        goto recover;
    }

    popts->globalexclude = nccmp_new_strlist(NC_MAX_VARS);
    if (! popts->globalexclude) {
        LOG_ERROR("Failed to allocate memory for global attribute exclusion list.\n");
        status = EXIT_FATAL;
        goto recover;
    }

    popts->excludeattlist = nccmp_new_strlist(NC_MAX_VARS);
    if (! popts->excludeattlist) {
        LOG_ERROR("Failed to allocate memory for attribute exclusion list.\n");
        status = EXIT_FATAL;
        goto recover;
    }

    popts->excludelist = nccmp_new_strlist(NC_MAX_VARS);
    if (! popts->excludelist) {
        LOG_ERROR("Failed to allocate memory for variable exclusion list.\n");
        status = EXIT_FATAL;
        goto recover;
    }

    popts->groupnames = nccmp_new_strlist(NC_MAX_VARS);
    if (! popts->groupnames) {
        LOG_ERROR("Failed to allocate memory for group names.\n");
        status = EXIT_FATAL;
        goto recover;
    }

    popts->variablelist = nccmp_new_strlist(NC_MAX_VARS);
    if (! popts->variablelist) {
        LOG_ERROR("Failed to allocate memory for variable list.\n");
        status = EXIT_FATAL;
        goto recover;
    }

    while ( (c = getopt_long(argc, (char*const*)argv, "A:bc:C:dDefFgG:hHimMn:Np:qPr:sSt:T:v:Vw:x:", long_options, 0))
          != -1
        ) {
        switch (c) {
        case 'A':
            nccmp_split_strlist_by_delim(optarg, popts->excludeattlist, ",");
            break;

        case 'b':
            popts->verbose = 1;
            break;

        case 'c':
            popts->var_diff_count = atoi(optarg);
            break;

        case 'C':
            popts->diff_count = atoi(optarg);
            break;

        case 'd':
            popts->data = 1;
            break;

        case 'D':
            popts->debug = 1;
            break;

        case 'e':
            popts->encoding = 1;
            break;

        case 'f':
            popts->force = 1;
            break;

        case 'F':
            popts->fortran = 1;
            break;

        case 'g':
            popts->global = 1;
            popts->metadata = 1;
            break;

        case 'G':
            nccmp_split_strlist_by_delim(optarg, popts->globalexclude, ",");
            popts->global = 1;
            popts->metadata = 1;
            break;

        case 'h':
            popts->history = 1;
            popts->global = 1;
            popts->metadata = 1;
            break;

        case 'H':
            popts->help = 1;
            break;

        case 'i':
            popts->info = 1;
            break;

        case 'm':
            popts->metadata = 1;
            break;

        case 'M':
            popts->missing = 1;
            break;

        case 'n':
            #ifdef HAVE_PTHREAD
            popts->threads = atoi(optarg);
            #else
            LOG_WARN("pthreads not supported. Defaulting to single thread.\n");
            popts->threads = 1;
            #endif
            break;

        case 'N':
            popts->nanequal = 1;
            break;

        case 'p':
            popts->precision = XMALLOC(char,strlen(optarg) + 1);
            strcpy(popts->precision, optarg);
            if (strncmp(popts->precision, "%", 1)) {
                LOG_ERROR("Invalid precision = %s\n", popts->precision);
                status = EXIT_FATAL;
                goto recover;
            }
            break;

        case 'P':
            #ifdef HAVE_NC_H
            popts->header_pad = 1;
            #else
            LOG_ERROR("Header pad feature unavailable. Recompile --with-netcdf pointing to NetCDF source path.\n");
            status = EXIT_FATAL;
            goto recover;
            #endif
            break;

        case 'q':
            popts->quiet = 1;
            break;

        case 'r':
            nccmp_split_strlist_by_delim(optarg, popts->groupnames, ",");
            break;

        case 's':
            popts->report_identical = 1;
            break;

        case 'S':
            popts->statistics = 1;
            break;

        case 't':
            popts->tolerance = strtod(optarg, NULL);
            popts->abstolerance = 1;
            if ( (errno == ERANGE) || (errno == EDOM))
            {
                LOG_ERROR("Specified tolerance cannot be used.\n");
                status = EXIT_FATAL;
                goto recover;
            }
            break;

        case 'T':
            popts->tolerance = strtod(optarg, NULL);
            popts->abstolerance = 0;
            if ( (errno == ERANGE) || (errno == EDOM))
            {
                LOG_ERROR("Specified tolerance cannot be used.\n");
                status = EXIT_FATAL;
                goto recover;
            }
            break;

        case 'v':
            nccmp_split_strlist_by_delim(optarg, popts->variablelist, ",");
            popts->variable = 1;
            break;

        case 'V':
            popts->version = 1;
            break;

        case 'w':
            warningtags = nccmp_new_strlist(NCCMP_WARN_NUM_TAGS);
            nccmp_split_strlist_by_delim(optarg, warningtags, ",");

            for(i=0; i < warningtags->size; ++i) {
                if (! nccmp_exists_in_strlist(valid_warnings, warningtags->items[i])) {
                    /* Warn user of any invalid warning tags they supplied. */
                    LOG_WARN("Warning tag \"%s\" is unsupported and will be ignored.\n", warningtags->items[i]);
                }
            }

            /* Look for defined tags that are supported. */
            if (nccmp_exists_in_strlist(warningtags, "all")) {
                popts->warn[NCCMP_W_ALL] = 1;
            } else if (nccmp_exists_in_strlist(warningtags, "format")) {
                popts->warn[NCCMP_W_FORMAT] = 1;
            } else if (nccmp_exists_in_strlist(warningtags, "eos")) {
                popts->warn[NCCMP_W_EOS] = 1;
            }

            break;

        case 'x':
            popts->exclude = 1;
            nccmp_split_strlist_by_delim(optarg, popts->excludelist, ",");
            break;

        case ':':
            LOG_ERROR("-%c without argument.\n", optopt);
            popts->help = 1;
            break;

        case '?':
            LOG_ERROR("Unknown argument %c.\n", optopt);
            popts->help = 1;
            break;
        }
    }

    if (popts->help) {
        print_usage();
        print_version();
        print_features();
        print_copyright();
        status = EXIT_SUCCESS;
        goto recover;
    }

    if (popts->version) {
        print_version();
        print_features();
        print_copyright();
        status = EXIT_SUCCESS;
        goto recover;
    }

    if (optind == argc) {
        LOG_ERROR("Missing operand after `%s'.\n", argv[argc - 1]);
        print_usage();
        status = EXIT_FATAL;
        goto recover;
    }

    if (!popts->data && !popts->metadata && !popts->info && !popts->encoding) {
        LOG_ERROR("Must supply at least one of these options: -d, -e, -i, -m.\n");
        print_usage();
        status = EXIT_FATAL;
        goto recover;
    }

    if ( popts->variable && popts->exclude ) {
        LOG_ERROR("Cannot combine -x and -v options.\n");
        print_usage();
        status = EXIT_FATAL;
        goto recover;
    }

    /* get filename arguments */
    argc -= optind;
    argv += optind;

    if (popts->info && (1 == argc)) {
        popts->file1 = XMALLOC(char,strlen(argv[0]) + 1);
        strcpy(popts->file1, argv[0]);
        popts->file2 = 0;
    } else if ((argc < 2)        ||
               (argv[0] == NULL) ||
               (argv[1] == NULL)) {
        LOG_ERROR("2 file arguments required.\n");
        print_usage();
        popts->file1 = NULL;
        popts->file2 = NULL;
        status = EXIT_FATAL;
        goto recover;
    } else {
        /* store filenames */
        popts->file1 = XMALLOC(char,strlen(argv[0]) + 1);
        popts->file2 = XMALLOC(char,strlen(argv[1]) + 1);
        strcpy(popts->file1, argv[0]);
        strcpy(popts->file2, argv[1]);
    }

    if (popts->history && !popts->global) {
        LOG_ERROR("-g required for -h option.\n");
        print_usage();
        status = EXIT_FATAL;
        goto recover;
    }

    if (!popts->metadata && (popts->history || popts->global) ) {
        LOG_ERROR("-m required for -g and -h options.\n");
        print_usage();
        status = EXIT_FATAL;
        goto recover;
    }

    if (popts->debug) {
        print_options(popts);
    }

recover:
    nccmp_free_strlist(& warningtags);
    nccmp_free_strlist(& valid_warnings);

    return status;
}
