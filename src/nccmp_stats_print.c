#include <log.h>
#include <nccmp_constants.h>
#include <nccmp_group.h>
#include <nccmp_stats_print.h>
#include <nccmp_utils.h>

const char *NCCMP_STATS_COLUMNS[] = {"Variable", "Group", "Count", "Sum", "AbsSum", "Min", "Max", "Range", "Mean", "StdDev"};

nccmp_darray_t* nccmp_compute_str_widths(nccmp_darray_t *array)
{
	int i;
	nccmp_darray_t *widths = nccmp_darray_create(array->num_items);

	for(i = 0; i < array->num_items; ++i) {
		nccmp_darray_append_int(widths, strlen( (char*) array->items[i] ));
	}

	return widths;
}

void nccmp_compute_column_max_widths(nccmp_darray_t *widths, int *max_widths)
{
    int i, col;
    if (!widths || !max_widths) {
        return;
    }

    for(i = 0; i < NCCMP_NUM_STATS_COLUMNS; ++i) {
        max_widths[i] = strlen(NCCMP_STATS_COLUMNS[i]);
    }

    for(i = 0; i < widths->num_items; ++i) {
        col = i % NCCMP_NUM_STATS_COLUMNS;
        max_widths[col] = nccmp_max_size_t(max_widths[col], *(int*) widths->items[i]);
    }
}

void nccmp_print_group_stats_array(nccmp_darray_t *array)
{
    nccmp_darray_t *strings = NULL, *widths = NULL;
    int max_widths[NCCMP_NUM_STATS_COLUMNS];

    nccmp_debug_print_group_stats_array(array); //FIXME

    LOG_DEBUG("FIXME nccmp_print_group_stats\n");
    strings = nccmp_stringify_group_stats_array(array);
    if (!strings) {
        return;
    }

    LOG_DEBUG("FIXME \n");
    widths = nccmp_compute_str_widths(strings);
    LOG_DEBUG("FIXME \n");
    nccmp_compute_column_max_widths(widths, max_widths);
    LOG_DEBUG("FIXME \n");
    nccmp_print_stats_table_header(max_widths);
    nccmp_print_stats_table_body(strings, widths, max_widths);
}

void nccmp_print_stats_table_header(int *max_widths)
{
	int col;

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
	#undef PRINT_PAD1
	#undef PRINT_STR1
}

void nccmp_print_stats_table_body(
		nccmp_darray_t *strings,
		nccmp_darray_t *widths,
		int *max_widths)
{
	int col, i;

    #define PRINT_PAD2() nccmp_print_pad(' ', (max_widths[col] - *(int*)widths->items[i]))
    #define PRINT_STR2() printf("%s", (char*) strings->items[i])
    for(col = 0, i = 0; i < strings->num_items; ++i) {
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

    nccmp_darray_destroy_deep(strings);
    nccmp_darray_destroy_deep(widths);

    #undef PRINT_PAD2
    #undef PRINT_STR2
}

void nccmp_print_pad(char c, int num)
{
    for(; --num >= 0; ) {
        printf("%c", c);
    }
}

void nccmp_stringify_group_stats(nccmp_group_stats_t *stats, nccmp_darray_t *result)
{
    if (stats) {
    	nccmp_stringify_var_stats_array(stats->var_stats, result);
    }
}

nccmp_darray_t* nccmp_stringify_group_stats_array(nccmp_darray_t *array)
{
    int i;
    nccmp_darray_t *result = nccmp_darray_create(NC_MAX_VARS);

    if (!array) {
        return result;
    }

    for(i = 0; i < array->num_items; ++i) {
    	nccmp_stringify_group_stats( (nccmp_group_stats_t*) array->items[i], result );
    }

    return result;
}

void nccmp_stringify_stats(nccmp_stats_t *stats, nccmp_darray_t *result)
{
    char tmp[NCCMP_MAX_STRINGS];

    LOG_DEBUG("FIXME nccmp_stringify_stat\n");

    nccmp_darray_append(result, stats->name);
    nccmp_darray_append(result, stats->group_name);
    snprintf(tmp, NCCMP_MAX_STRINGS, "%d", stats->count);
    nccmp_darray_append(result, tmp);
    snprintf(tmp, NCCMP_MAX_STRINGS, "%g", stats->sum);
    nccmp_darray_append(result, tmp);
    snprintf(tmp, NCCMP_MAX_STRINGS, "%g", stats->abs_sum);
    nccmp_darray_append(result, tmp);
    snprintf(tmp, NCCMP_MAX_STRINGS, "%g", stats->min);
    nccmp_darray_append(result, tmp);
    snprintf(tmp, NCCMP_MAX_STRINGS, "%g", stats->max);
    nccmp_darray_append(result, tmp);
    snprintf(tmp, NCCMP_MAX_STRINGS, "%g", stats->max - stats->min);
    nccmp_darray_append(result, tmp);
    snprintf(tmp, NCCMP_MAX_STRINGS, "%g", nccmp_compute_var_stats_mean(stats));
    nccmp_darray_append(result, tmp);
    snprintf(tmp, NCCMP_MAX_STRINGS, "%g", nccmp_compute_var_stats_stdev(stats));
    nccmp_darray_append(result, tmp);
}

void nccmp_stringify_stats_array(nccmp_darray_t *array, nccmp_darray_t *result)
{
    int i;
    if (array) {
    	for(i = 0; i < array->num_items; ++i) {
    		nccmp_stringify_stats( (nccmp_stats_t*) array->items[i], result );
    	}
    }
}

void nccmp_stringify_var_stats(nccmp_var_stats_t *stats, nccmp_darray_t *result)
{
    if (stats) {
    	nccmp_stringify_stats_array(stats->stats, result);
    }
}

void nccmp_stringify_var_stats_array(nccmp_darray_t *array, nccmp_darray_t *result)
{
    int i;
    if (array) {
    	for(i = 0; i < array->num_items; ++i) {
    		nccmp_stringify_var_stats( (nccmp_var_stats_t*) array->items[i], result );
    	}
    }
}
