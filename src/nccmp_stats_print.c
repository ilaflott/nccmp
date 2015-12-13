#include <log.h>
#include <nccmp_constants.h>
#include <nccmp_group.h>
#include <nccmp_stats_print.h>
#include <nccmp_utils.h>

const char *NCCMP_STATS_COLUMNS[] = {"Variable", "Group", "Count", "Sum", "AbsSum", "Min", "Max", "Mean", "StdDev"};

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

void nccmp_print_group_stats(nccmp_group_stats_t *array,
        nccmp_group_t *groups, nccmp_var_t *vars,
        nccmp_user_types_t *fields, int num_vars)
{
    int i, col;
    nccmp_strlist_t *strings = NULL;
    int *widths = NULL;
    int max_widths[NCCMP_NUM_STATS_COLUMNS];

    strings = nccmp_stringify_group_stats(array, groups, vars, fields, num_vars);
    if (!strings) {
        return;
    }

    widths = nccmp_compute_strlist_lengths(strings);
    if (!widths) {
        return;
    }

    nccmp_compute_stats_column_widths(strings, max_widths);

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

nccmp_strlist_t* nccmp_stringify_group_stats(
        nccmp_group_stats_t *group_stats,
        nccmp_group_t *groups,
        nccmp_var_t *vars,
        nccmp_user_types_t *fields,
        int num_vars
        )
{
    int num_valid_rows = 0;
    char tmpstr[NCCMP_MAX_STRINGS];
    nccmp_strlist_t *result = NULL;
    int groupi;
    nccmp_group_t *group;
    int stati;
    nccmp_stats_t *stat;

    memset(tmpstr, 0, NCCMP_MAX_STRINGS);

    if (!group_stats || (0 == group_stats->num_groups) ) {
        return result;
    }

    /* Count items with diffs. */
    for(groupi = 0; groupi < group_stats->num_groups; ++groupi) {
        for(stati = 0; stati < group_stats->num_vars[groupi]; ++stati) {
            if (0 < group_stats->var_stats[groupi][stati].count) {
                ++num_valid_rows;
            }
        }
        for(stati = 0; stati < group_stats->num_fields[groupi]; ++stati) {
            if (0 < group_stats->field_stats[groupi][stati].count) {
                ++num_valid_rows;
            }
        }
    }

    result = nccmp_new_strlist(num_valid_rows * NCCMP_NUM_STATS_COLUMNS);

    #define APPEND(S)                                                           \
        if (nccmp_add_to_strlist(result, S)) {                                  \
            LOG_ERROR("Failed to add string to statistics output.\n");          \
            goto recover;                                                       \
        }

    #define GET_VAR_NAME(I) vars[I].name
    #define GET_FIELD_NAME(I) nccmp_find_field_by_unique_id(fields, I)->tree_name

    #define APPEND_ITEMS(STATS, NUM_STATS, GET_NAME)                                \
        for(stati = 0; stati < NUM_STATS; ++stati) {                                \
            stat = & (STATS[groupi][stati]);                                        \
            if (0 > stat->count) {                                                  \
                continue;                                                           \
            }                                                                       \
            APPEND(GET_NAME(stati));                                                \
            APPEND(group->fullname);                                                \
            snprintf(tmpstr, NCCMP_MAX_STRINGS, "%d", stat->count);                 \
            APPEND(tmpstr);                                                         \
            snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", stat->sum);                   \
            APPEND(tmpstr);                                                         \
            snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", stat->abs_sum);               \
            APPEND(tmpstr);                                                         \
            snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", stat->min);                   \
            APPEND(tmpstr);                                                         \
            snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", stat->max);                   \
            APPEND(tmpstr);                                                         \
            snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", nccmp_compute_var_stats_mean(stat));  \
            APPEND(tmpstr);                                                                 \
            snprintf(tmpstr, NCCMP_MAX_STRINGS, "%g", nccmp_compute_var_stats_stdev(stat)); \
            APPEND(tmpstr);                                                                 \
        }

    for(groupi = 0; groupi < group_stats->num_groups; ++groupi) {
        group = nccmp_find_group_by_id(groups, group_stats->group_ids[groupi]);
        APPEND_ITEMS(group_stats->var_stats,   group_stats->num_vars[groupi],   GET_VAR_NAME);
        APPEND_ITEMS(group_stats->field_stats, group_stats->num_fields[groupi], GET_FIELD_NAME);
    }

recover:
    #undef APPEND
    #undef APPEND_STAT
    #undef GET_FIELD_NAME
    #undef GET_VAR_NAME

    return result;
}

