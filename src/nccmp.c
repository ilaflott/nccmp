/*
   Copyright (C) 2004-2007,2009,2010,2012 Remik Ziemlinski @ noaa gov

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

#include <log.h>
#include <math.h>
#include <nccmp.h>
#include <nccmp_dim.h>
#include <nccmp_group.h>
#include <nccmp_io.h>
#include <nccmp_metadata.h>
#include <nccmp_nc_type.h>
#include <nccmp_ncinfo.h>
#include <nccmp_opt.h>
#include <nccmp_stats_print.h>
#include <nccmp_user_type.h>
#include <nccmp_utils.h>
#include <nccmp_var.h>
#include <netcdf.h>


int nccmp_run(nccmp_state_t* state)
{
    int status = EXIT_SUCCESS, last_status = EXIT_SUCCESS;

    io_init();

    if (state->opts.verbose) {
        LOG_INFO_CHOOSE(state->opts.color, "Opening input files.\n");
    }

    status = nccmp_open_files(state);
    last_status = status ? status : last_status;
    if (last_status) {
        goto recover;
    }

    if (state->opts.debug) {
        LOG_DEBUG_CHOOSE(state->opts.color, "status = %d\n", status);
    }

    if (state->opts.info)
    {
        if (state->thread_groups1) {
            status = nccmp_dump_info(state, state->thread_groups1[0].ncid);
            last_status = status ? status : last_status;
            if (last_status) {
                goto recover;
            }
        }

        if (state->thread_groups2) {
            status = nccmp_dump_info(state, state->thread_groups2[0].ncid);
            last_status = status ? status : last_status;
            if (last_status) {
                goto recover;
            }
        }
    }

    if (!state->thread_groups1 || !state->thread_groups2) {
        // Nothing to compare, so abort.
        goto recover;
    }

    if (state->opts.verbose) {
        LOG_INFO_CHOOSE(state->opts.color, "Comparing file formats.\n");
    }

    status = nccmp_file_formats(& state->opts, state->thread_groups1[0].ncid,
                state->thread_groups2[0].ncid);
    if (state->opts.debug) {
        LOG_DEBUG_CHOOSE(state->opts.color, "status = %d\n", status);
    }
    last_status = status ? status : last_status;
    if (last_status && !state->opts.force) {
        goto recover;
    }

    if (state->opts.verbose) {
        LOG_INFO_CHOOSE(state->opts.color, "Comparing groups.\n");
    }

    status = nccmp_cmp_groups(state);
    if (state->opts.debug) {
        LOG_DEBUG_CHOOSE(state->opts.color, "status = %d\n", status);
    }
    last_status = status ? status : last_status;
    if (last_status && !state->opts.force) {
        goto recover;
    }

    if (state->opts.verbose) {
      LOG_INFO_CHOOSE(state->opts.color, "Comparisons complete.\n");
    }

    if (state->opts.statistics) {
        nccmp_print_group_stats_array(state->stats);
    }

recover:

    if (state->opts.debug) {
      LOG_DEBUG_CHOOSE(state->opts.color, "last_status = %d\n", last_status);
    }

    io_destroy();
    return last_status;
}

int nccmp_main(int argc, const char** argv)
{
    int status = EXIT_SUCCESS;
    nccmp_state_t state;

    nccmp_init_state(& state);

    status = nccmp_opt_parse(argc, argv, & state.opts);

    if (EXIT_SUCCESS != status) {
        goto recover;
    }

    if (state.opts.verbose) {
        LOG_INFO_CHOOSE(state.opts.color, "Command-line options parsed.\n");
    }

    status = nccmp_run(& state);

recover:
    if (state.opts.report_identical &&
        (EXIT_SUCCESS == status) &&
        !state.opts.help &&
        !state.opts.version) {
        fprintf(stdout, "Files \"%s\" and \"%s\" are identical.\n",
                state.opts.file1, state.opts.file2);
    }

    nccmp_destroy_state(& state);

    return status;
}

int main(int argc, const char** argv)
{
    return nccmp_main(argc, argv);
}
