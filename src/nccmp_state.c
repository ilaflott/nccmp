#include <log.h>
#include <nccmp_state.h>
#include <nccmp_error.h>
#include <nccmp_group.h>
#include <nccmp_utils.h>

int nccmp_check_diff_count(nccmp_state_t* state, int varid)
{
    return (state->opts.var_diff_count &&
                (state->var_diff_counts[varid] >= state->opts.var_diff_count))
           ||
           (state->opts.diff_count &&
                (state->global_diff_count >= state->opts.diff_count));
}

count_limit_type nccmp_inc_diff_count(nccmp_state_t* state, int varid)
{
    count_limit_type result;
    int delta = -1;

    #if HAVE_PTHREAD
        pthread_mutex_lock(& state->mutex_diff_count);
    #endif
    state->var_diff_counts[varid] += 1;
    state->global_diff_count += 1;
    if (state->opts.var_diff_count) {
        delta = state->var_diff_counts[varid] - state->opts.var_diff_count;
    } else if (state->opts.diff_count) {
        delta = state->global_diff_count - state->opts.diff_count;
    }
    #if HAVE_PTHREAD
        pthread_mutex_unlock(& state->mutex_diff_count);
    #endif

    if (0 == delta) {
        result = COUNT_EQUAL;
    } else if (delta > 0) {
        result = COUNT_OVER;
    } else {
        result = COUNT_UNDER;
    }
    return result;
}

void nccmp_destroy_state_groups(struct nccmp_group_t **thread_groups, int *nthread_groups)
{
    int i, status;

    for(i = 0; i < *nthread_groups; ++i) {
        status = nc_close((*thread_groups)[i].id);
        HANDLE_NC_ERROR(status);
        nccmp_destroy_group(& (*thread_groups)[i]);
    }
    
    nccmp_destroy_groups(thread_groups, nthread_groups);
}

void nccmp_destroy_state(nccmp_state_t *state)
{
    nccmp_destroy_state_groups(& state->thread_groups1, & state->nthread_groups1);
    nccmp_destroy_state_groups(& state->thread_groups2, & state->nthread_groups2);

    nccmp_opt_free(& state->opts);

    nccmp_destroy_dims(& state->dims1, & state->ndims1);
    nccmp_destroy_dims(& state->dims2, & state->ndims2);

    #if HAVE_PTHREAD
    pthread_mutex_destroy(& state->mutex_diff_count);
    #endif

    nccmp_destroy_user_types(state->types1);
    nccmp_destroy_user_types(state->types2);

    var_info_destroy(& state->vars1, & state->nvars1);
    var_info_destroy(& state->vars2, & state->nvars2);

    XFREE(state->var_diff_counts);

    state->global_diff_count = 0;
    state->nrecs1   = state->nrecs2   = 0;
    state->recid1   = state->recid2 = 0;
}

void nccmp_init_state(nccmp_state_t *state)
{
    if (!state) {
        return;
    }

    state->vars1 = 0;
    state->vars2 = 0;
    state->dims1 = 0;
    state->dims2 = 0;
    state->types1 = 0;
    state->types2 = 0;
    state->thread_groups1 = 0;
    state->thread_groups2 = 0;

    state->nrecs1 = 0;
    state->nrecs2 = 0;
    state->nvars1 = 0;
    state->nvars2 = 0;
    state->ndims1 = 0;
    state->ndims2 = 0;
    state->nthread_groups1 = 0;
    state->nthread_groups2 = 0;

    state->recid1 = -1;
    state->recid2 = -1;

    state->global_diff_count = 0;
    state->var_diff_counts = 0;

    nccmp_opt_init(& state->opts);

    #if HAVE_PTHREAD
    pthread_mutex_init(& state->mutex_diff_count, NULL);
    #endif
}

int nccmp_open_file(nccmp_state_t* state, nccmp_group_t** groups,
        const char* filename, int* ngroups)
{
    int i, status = EXIT_SUCCESS;

    if (filename)
    {
        if (state->opts.verbose) {
          LOG_INFO("Opening %s\n", filename);
        }

        *ngroups = state->opts.threads;
        *groups = nccmp_create_groups(*ngroups);
        for(i = 0; i < *ngroups; ++i) {
            status = nc_open(filename, NC_NOWRITE, & ((*groups)[i].id) );
            HANDLE_NC_ERROR(status);
            status = nccmp_get_group_info((*groups)[i].id, & (*groups)[i]);
            if (status) {
                goto recover;
            }
        }
    }

recover:
    return status;
}

int nccmp_open_files(nccmp_state_t* state)
{
    int status;

    status = nccmp_open_file(state, & state->thread_groups1, state->opts.file1,
                & state->nthread_groups1);
    if (status) {
        goto recover;
    }

    status = nccmp_open_file(state, & state->thread_groups2, state->opts.file2,
                & state->nthread_groups2);

recover:
    return status;
}

void var_diff_counts_init(nccmp_state_t* state)
{
    int max_nvars;

    if (state->var_diff_counts) {
        XFREE(state->var_diff_counts);
    }

    max_nvars = nccmp_max(state->nvars1, state->nvars2);
    state->var_diff_counts = XMALLOC(int, max_nvars);
    memset(state->var_diff_counts, 0, max_nvars * sizeof(int));
}
