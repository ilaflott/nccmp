#ifndef NCCMP_STATS_PRINT_H
#define NCCMP_STATS_PRINT_H 1

#include <nccmp_strlist.h>
#include <nccmp_stats.h>

extern const char *NCCMP_STATS_COLUMNS[];
#define NCCMP_NUM_STATS_COLUMNS 10

nccmp_darray_t* nccmp_compute_str_widths(nccmp_darray_t *array);

void nccmp_compute_column_max_widths(nccmp_darray_t *widths, int *max_widths);

void nccmp_debug_print_group_stats(nccmp_group_stats_t *group_stats);

void nccmp_debug_print_group_stats_array(nccmp_darray_t *array);

void nccmp_print_group_stats_array(nccmp_darray_t *array);

/* Print a character 'num' times to stdout. */
void nccmp_print_pad(char c, int num);

void nccmp_print_stats_table_body(nccmp_darray_t *strings, nccmp_darray_t *widths, int *max_widths);

void nccmp_print_stats_table_header(int *max_widths);

void nccmp_stringify_group_stats(nccmp_group_stats_t *array, nccmp_darray_t *result);

/* Returns flat string array for all columns for all variables with diffs. */
nccmp_darray_t* nccmp_stringify_group_stats_array(nccmp_darray_t *array);

/* Append row of stat strings to result. */
void nccmp_stringify_stats(nccmp_stats_t *stat, nccmp_darray_t *result);

void nccmp_stringify_stats_array(nccmp_darray_t *array, nccmp_darray_t *result);

void nccmp_stringify_var_stats(nccmp_var_stats_t *array, nccmp_darray_t *result);

void nccmp_stringify_var_stats_array(nccmp_darray_t *array, nccmp_darray_t *result);

#endif
