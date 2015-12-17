#include <log.h>
#include <nccmp_constants.h>
#include <nccmp_group.h>
#include <nccmp_stats_print.h>
#include <nccmp_utils.h>

const char *NCCMP_STATS_COLUMNS[] = {"Variable", "Group", "Count", "Sum", "AbsSum", "Min", "Max", "Range", "Mean", "StdDev"};

void nccmp_compute_stats_column_widths(nccmp_strlist_t *strings, int *widths)
{
    int i, col;
    if (!strings || !widths) {
        return;
    }

    for(i = 0; i < NCCMP_NUM_STATS_COLUMNS; ++i) {
        widths[i] = strlen(NCCMP_STATS_COLUMNS[i]);
    }

    for(i = 0; i < strings->size; ++i) {
        col = i % NCCMP_NUM_STATS_COLUMNS;
        widths[col] = nccmp_max_size_t(widths[col], strlen(strings->items[i]));
    }
}

void nccmp_print_group_stats(nccmp_group_stats_t *group_stats)
{
    int i, col;
    nccmp_strlist_t *strings = NULL;
    int *widths = NULL;
    int max_widths[NCCMP_NUM_STATS_COLUMNS];

    strings = nccmp_stringify_group_stats(group_stats);
    if (!strings) {
        return;
    }

    LOG_DEBUG("\n");
    widths = nccmp_compute_strlist_lengths(strings);
    if (!widths) {
        return;
    }
    LOG_DEBUG("\n");
    nccmp_compute_stats_column_widths(strings, max_widths);
    LOG_DEBUG("\n");
    /* Header */
    #define PRINT_PAD1() nccmp_print_pad(' ', (max_widths[col] - strlen(NCCMP_STATS_COLUMNS[col])));
    #define PRINT_STR1() printf("%s", NCCMP_STATS_COLUMNS[col])
    for(col = 0; col < NCCMP_NUM_STATS_COLUMNS; ++col) {
        if (0 < col) {
            printf(" ");
        }
        switch (col) {
            case 0:
            case 1: /* Left justify first two columns. */
                PRINT_STR1();
                PRINT_PAD1();
                break;
            default: /* Right justify. */
                PRINT_PAD1();
                PRINT_STR1();
                break;
        }
    }
    printf("\n");

    /* Body */
    #define PRINT_PAD2() nccmp_print_pad(' ', (max_widths[col] - widths[i]))
    #define PRINT_STR2() printf("%s", strings->items[i])
    for(col = 0, i = 0; i < strings->size; ++i) {
        col = i % NCCMP_NUM_STATS_COLUMNS;
        if (0 < col) {
            printf(" ");
        }
        switch (col) {
            case 0:
            case 1: /* Left justify first two columns. */
                PRINT_STR2();
                PRINT_PAD2();
                break;
            default: /* Right justify. */
                PRINT_PAD2();
                PRINT_STR2();
                break;
        }
        if ( NCCMP_NUM_STATS_COLUMNS == (col+1) ) {
            printf("\n");
        }
    }

    nccmp_free_strlist(& strings);
    XFREE(widths);

    #undef PRINT_PAD1
    #undef PRINT_PAD2
    #undef PRINT_STR1
    #undef PRINT_STR2
}

void nccmp_print_pad(char c, int num)
{
    for(; --num >= 0; ) {
        printf("%c", c);
    }
}

nccmp_strlist_t* nccmp_stringify_group_stats(nccmp_group_stats_t *group_stats)
{
    int num_non_empty_stats = 0, group_i = 0, stat_i = 0, var_i = 0;
    nccmp_strlist_t *result = NULL;

    if (!group_stats || !group_stats->num_groups) {
        return result;
    }

    /* Count items with diffs. */
    for(group_i = 0; group_i < group_stats->num_groups; ++group_i) {
        for(var_i = 0; var_i < group_stats->num_vars[group_i]; ++var_i) {
            for(stat_i = 0; stat_i < group_stats->num_stats[group_i][var_i]; ++stat_i) {
                if (0 < group_stats->stats[group_i][var_i][stat_i].count) {
                    ++num_non_empty_stats;
                }
            }
        }
    }

    if (!num_non_empty_stats) {
        return result;
    }

    result = nccmp_new_strlist(num_non_empty_stats * NCCMP_NUM_STATS_COLUMNS);

    for(group_i = 0; group_i < group_stats->num_groups; ++group_i) {
        for(var_i = 0; var_i < group_stats->num_vars[group_i]; ++var_i) {
            for(stat_i = 0; stat_i < group_stats->num_stats[group_i][var_i]; ++stat_i) {
                if (0 < group_stats->stats[group_i][var_i][stat_i].count) {
                    nccmp_stringify_stat(& group_stats->stats[group_i][var_i][stat_i], result);
                }
            }
        }
    }

    LOG_DEBUG("FIXME\n");

    return result;
}

void nccmp_stringify_stat(nccmp_stats_t *stat, nccmp_strlist_t *result)
{
    char tmpstr[NCCMP_MAX_STRINGS];

    LOG_DEBUG("FIXME\n");

    #define APPEND(S)                                                   \
        if (nccmp_add_to_strlist(result, S)) {                          \
            LOG_ERROR("Failed to add string to statistics output.\n");  \
            return;                                                     \
        }

    LOG_DEBUG("FIXME\n");
    APPEND(stat->name);
    APPEND(stat->group_name);
    LOG_DEBUG("FIXME\n");
    snprintf(tmpstr, NCCMP_MAX_STRINGS, "%d", stat->count);
    APPEND(tmpstr);
    snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", stat->sum);
    APPEND(tmpstr);
    snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", stat->abs_sum);
    APPEND(tmpstr);
    snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", stat->min);
    APPEND(tmpstr);
    snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", stat->max);
    APPEND(tmpstr);
    snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", stat->max - stat->min);
    APPEND(tmpstr);
    snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", nccmp_compute_var_stats_mean(stat));
    APPEND(tmpstr);
    snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", nccmp_compute_var_stats_stdev(stat));
    APPEND(tmpstr);

    #undef APPEND
}
