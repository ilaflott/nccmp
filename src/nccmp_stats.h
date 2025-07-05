#ifndef NCCMP_STATS_H
#define NCCMP_STATS_H 1

#include <config.h>
#include <nccmp_user_type.h>
#include <nccmp_var.h>

#if HAVE_PTHREAD
    #include <pthread.h>
#endif

/* Accumulates diff statistics from comparisons. */
typedef struct nccmp_stats_t {
    int    count;
    double min_diff;
    double max_diff;
    double sum_diff;                /* Helps compute mean and stdev. */
    double abs_sum_diff;            /* L1 Norm. */
    double abs_sum_diff_over_truth; /* Helps compute L1 Relative Norm. */
    double sum_diff_squares;        /* Helps compute stdev and L2 Norm. */
    double sum_truth_squares;       /* Helps compute L2 Relative Norm denominator. */
    char   *name;                   /* Var or dotted field tree name. */
    char   *group_name;             /* Name of group it belongs to. */

    #if HAVE_PTHREAD
        pthread_mutex_t lock; /* Lock stats updates. */
    #endif
} nccmp_stats_t;

typedef struct nccmp_var_stats_t {
    int var_id;
    /* Array of nccmp_stats_t indexed by user-type unique_id or singleton if var has no fields. */
    nccmp_darray_t *stats;
} nccmp_var_stats_t;

/* Contains stats arrays for all groups in a file. */
typedef struct nccmp_group_group_t {
    int group_id;
    nccmp_darray_t *var_stats; /* Array of nccmp_vars_stats_t indexed by varid in group. */
} nccmp_group_stats_t;

double nccmp_compute_var_stats_mean(nccmp_stats_t *data);

double nccmp_compute_var_stats_stdev(nccmp_stats_t *data);

double nccmp_compute_var_stats_l1_relative(nccmp_stats_t *data);

double nccmp_compute_var_stats_l2(nccmp_stats_t *data);

double nccmp_compute_var_stats_l2_relative(nccmp_stats_t *data);

nccmp_group_stats_t* nccmp_create_group_stats(int group_id,
		const char *group_name, nccmp_var_t *vars, int num_vars,
		nccmp_darray_t *types);

nccmp_stats_t* nccmp_create_stats(const char *name, const char *group_name);

/* For a given variable and group, create new stats arrays.
 * Each item in output array is indexed by varid.
 * Each var will then have an array for a stat per field if it is compound,
 * or just a signleton array with one stats item for the variable itself.
 */
nccmp_var_stats_t* nccmp_create_var_stats(nccmp_var_t *var,
		nccmp_darray_t *types, const char *group_name);

void nccmp_clear_var_stats_array(nccmp_stats_t *array, int n);

void nccmp_destroy_group_stats(nccmp_group_stats_t *group_stats);

void nccmp_destroy_group_stats_array(nccmp_darray_t *array);

void nccmp_destroy_stats(nccmp_stats_t *stats);

void nccmp_destroy_stats_array(nccmp_darray_t *array);

void nccmp_destroy_var_stats_array(nccmp_darray_t *array);

void nccmp_update_stats(nccmp_stats_t *stats, double truth, double prediction);

#endif
