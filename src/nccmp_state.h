#ifndef NCCMP_STATE_H
#define NCCMP_STATE_H

#include <netcdf.h>
#include <nccmp_dim.h>
#include <nccmp_opt.h>
#include <nccmp_user_type.h>
#include <nccmp_var.h>

#if HAVE_PTHREAD
    #include <pthread.h>
#endif

struct nccmp_group_t;  /* Forward declaration. */

typedef struct nccmp_state_t {
    nccmp_dim_t         *dims1;
    nccmp_dim_t         *dims2;
    int                 global_diff_count;
    int                 ndims1;
    int                 ndims2;
    int                 nthread_groups1;
    int                 nthread_groups2;
    size_t              nrecs1;
    size_t              nrecs2;
    int                 nvars1;
    int                 nvars2;
    int                 recid1;
    int                 recid2;
    nccmp_opt_t         opts;
    #if HAVE_PTHREAD
    pthread_mutex_t     mutex_diff_count;
    #endif
    nccmp_user_types_t  *types1;
    nccmp_user_types_t  *types2;
    nccmp_var_t         *vars1;
    nccmp_var_t         *vars2;
    int                 *var_diff_counts;

    /* Each thread will have own group tree (ncid filehandles) since api not thread safe. */
    struct nccmp_group_t *thread_groups1;
    struct nccmp_group_t *thread_groups2;
} nccmp_state_t;

/* Returns non-zero if globar or var count is above or equal to threshold. */
int nccmp_check_diff_count(nccmp_state_t* state, int varid);

void nccmp_destroy_state(nccmp_state_t *state);

/* Increment and check count (global or variable with varid). */
count_limit_type nccmp_inc_diff_count(nccmp_state_t* state, int varid);

void nccmp_init_state(nccmp_state_t *state);

/* Should only be called once per program instance. */
int nccmp_open_files(nccmp_state_t* state);

void var_diff_counts_init(nccmp_state_t* state);

#endif
