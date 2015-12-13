#ifndef NCCMP_STATS_H
#define NCCMP_STATS_H 1

#include <config.h>

#if HAVE_PTHREAD
    #include <pthread.h>
#endif

/* Accumulates diff statistics from comparisons. */
typedef struct nccmp_stats_t {
    int    count;
    double min;
    double max;
    double sum;          /* Helps compute mean and stdev. */
    double abs_sum;
    double sum_squares;  /* Helps compute stdev. */

    #if HAVE_PTHREAD
        pthread_mutex_t lock; /* Lock stats updates. */
    #endif
} nccmp_stats_t;

/* Contains var stats arrays for all groups in a file. */
typedef struct nccmp_group_stats_t {
    int num_groups;
    int *group_ids; /* Has length num_groups. */
    int *num_vars;  /* Number of vars each group has. */
    int *num_fields;  /* Number of leaf fields each group has. */
    nccmp_stats_t **var_stats;
    nccmp_stats_t **field_stats;
} nccmp_group_stats_t;

double nccmp_compute_var_stats_mean(nccmp_stats_t *data);

double nccmp_compute_var_stats_stdev(nccmp_stats_t *data);

void nccmp_clear_var_stats_array(nccmp_stats_t *array, int n);

nccmp_group_stats_t* nccmp_create_group_stats_array(int num_groups);

void nccmp_init_group_stats(nccmp_group_stats_t *group_array, int groupid,
        int num_vars, int num_fields);

nccmp_stats_t* nccmp_find_var_stats_array(nccmp_group_stats_t *group_stats, int group_id);

void nccmp_destroy_group_stats(nccmp_group_stats_t *array);

void nccmp_print_var_stat_pointers(nccmp_stats_t *stats, int groupid, int num_vars);

void nccmp_update_stats(nccmp_stats_t *stats, double delta);

#endif
