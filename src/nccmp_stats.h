#ifndef NCCMP_STATS_H
#define NCCMP_STATS_H 1

#if HAVE_PTHREAD
    #include <pthread.h>
#endif

/* Accumulates diff statistics from comparisons. */
typedef struct nccmp_var_stats_t {
    int    count;
    double min;
    double max;
    double sum;          /* Helps compute mean and stdev. */
    double abs_sum;
    double sum_squares;  /* Helps compute stdev. */

    #if HAVE_PTHREAD
        pthread_mutex_t lock; /* Lock stats updates. */
    #endif
} nccmp_var_stats_t;

/* Container for array of var stats for a group. */
typedef struct nccmp_var_stats_array_t {
    int num_items;
    nccmp_var_stats_t *items; /* Index into array is varid in the group. */
} nccmp_var_stats_array_t;

/* Contains var stats arrays for all groups in a file. */
typedef struct nccmp_group_stats_array_t {
    int num_items;
    int *group_ids; /* Has length num_items, one item per group. */
    nccmp_var_stats_array_t *items;
} nccmp_group_stats_array_t;

double nccmp_compute_var_stats_mean(nccmp_var_stats_t *data);

double nccmp_compute_var_stats_stdev(nccmp_var_stats_t *data);

void nccmp_clear_var_stats_array(nccmp_var_stats_array_t *array);

nccmp_group_stats_array_t* nccmp_create_group_stats_array(int num_groups);

void nccmp_init_group_stats(nccmp_group_stats_array_t *group_array, int groupid, int num_vars);

nccmp_var_stats_array_t* nccmp_find_group_var_stats_array(nccmp_group_stats_array_t *array, int group_id);

void nccmp_destroy_group_stats_array(nccmp_group_stats_array_t *array);

#endif
