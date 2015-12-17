#ifndef NCCMP_STATS_PRINT_H
#define NCCMP_STATS_PRINT_H 1

#include <nccmp_strlist.h>
#include <nccmp_stats.h>

extern const char *NCCMP_STATS_COLUMNS[];
#define NCCMP_NUM_STATS_COLUMNS 10

void nccmp_compute_stats_column_widths(nccmp_strlist_t *strings, int *widths);

void nccmp_print_group_stats(nccmp_group_stats_t *group_stats);

/* Print a character 'num' times to stdout. */
void nccmp_print_pad(char c, int num);

/* Append row of stat strings to result. */
void nccmp_stringify_stat(nccmp_stats_t *stat, nccmp_strlist_t *result);

/* Returns flat string array for all columns for all variables with diffs. */
nccmp_strlist_t* nccmp_stringify_group_stats(nccmp_group_stats_t *group_stats);

#endif
