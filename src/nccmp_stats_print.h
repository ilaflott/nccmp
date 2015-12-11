#ifndef NCCMP_STATS_PRINT_H
#define NCCMP_STATS_PRINT_H 1

#include <nccmp_strlist.h>
#include <nccmp_stats.h>

extern const char *NCCMP_STATS_COLUMNS[];
#define NCCMP_NUM_STATS_COLUMNS 9

void nccmp_compute_stats_column_widths(nccmp_strlist_t *strings, int *widths);

void nccmp_print_group_stats_array(nccmp_group_stats_array_t *array,
        nccmp_group_t *groups, nccmp_var_t *vars);

/* Print a character 'num' times to stdout. */
void nccmp_print_pad(char c, int num);

/* Returns flat string array for all columns for all variables with diffs. */
nccmp_strlist_t* nccmp_stringify_group_stats_array(nccmp_group_stats_array_t *array,
        nccmp_group_t *groups, nccmp_var_t *vars);

#endif
