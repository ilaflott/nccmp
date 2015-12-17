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
    double min;
    double max;
    double sum;            /* Helps compute mean and stdev. */
    double abs_sum;
    double sum_squares;    /* Helps compute stdev. */
    char   *name;          /* Var or field name. */
    char   *group_name;    /* Name of group it belongs to. */

    #if HAVE_PTHREAD
        pthread_mutex_t lock; /* Lock stats updates. */
    #endif
} nccmp_stats_t;

/* Contains stats arrays for all groups in a file. */
typedef struct nccmp_group_stats_t {
    int num_groups;
    int *group_ids; /* Has length num_groups. */
    int *num_vars;  /* Number of vars each group has. */
    int **num_stats;  /* Number of stats for a variable in group. Compound types will have more than 1. [group_index][varid] */
    nccmp_stats_t ***stats; /* Indexed by [group_index][varid][unique_user_type_id] */
} nccmp_group_stats_t;

double nccmp_compute_var_stats_mean(nccmp_stats_t *data);

double nccmp_compute_var_stats_stdev(nccmp_stats_t *data);

nccmp_group_stats_t* nccmp_create_group_stats_array(int num_groups);

void nccmp_clear_var_stats_array(nccmp_stats_t *array, int n);

void nccmp_destroy_group_stats(nccmp_group_stats_t *array);

void nccmp_destroy_stats_array(nccmp_stats_t *stats, int n);

void nccmp_destroy_stats_arrays(nccmp_stats_t **stats, int num_vars, int *num_fields);

nccmp_stats_t** nccmp_find_group_var_stats(nccmp_group_stats_t *group_stats, int group_id);

void nccmp_init_group_stats(nccmp_group_stats_t *group_stats,
        int groupid, const char* group_name,
        nccmp_var_t *vars, int num_vars,
        nccmp_field_hashmap_t *field_hashmap);

void nccmp_print_var_stat_pointers(nccmp_stats_t *stats, int groupid, int num_vars);

void nccmp_set_var_stat_names(nccmp_stats_t *var_stats_array, int num_stats,
        nccmp_var_t *var, nccmp_field_hashmap_t *field_hashmap,
        const char* group_name);

void nccmp_update_stats(nccmp_stats_t *stats, double delta);

#endif
