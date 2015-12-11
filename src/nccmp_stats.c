#include <float.h>
#include <log.h>
#include <math.h>
#include <nccmp_stats.h>
#include <nccmp_common.h>

double nccmp_compute_var_stats_mean(nccmp_var_stats_t *data)
{
    if (data && (data->count > 0) ) {
        return data->sum / data->count;
    } else {
        return 0.0;
    }
}

double nccmp_compute_var_stats_stdev(nccmp_var_stats_t *data)
{
    if (data && (data->count > 0) ) {
        return sqrt( (data->sum_squares - (data->sum * data->sum)/data->count) /
                     (data->count - 1) );
    } else {
        return 0.0;
    }
}

void nccmp_clear_var_stats_array(nccmp_var_stats_array_t *array)
{
    int i;
    if (!array) {
        return;
    }

    for(i = 0; i < array->num_items; ++i) {
        array->items[i].count = 0;
        array->items[i].min = INFINITY;
        array->items[i].max = -INFINITY;
        array->items[i].sum = 0.0;
        array->items[i].abs_sum = 0.0;
        array->items[i].sum_squares = 0.0;
        LOG_DEBUG("FIXME i=%d\n", i);
    }
}

nccmp_group_stats_array_t* nccmp_create_group_stats_array(int num_groups)
{
    nccmp_group_stats_array_t *result = XMALLOC(nccmp_group_stats_array_t, 1);
    result->num_items = num_groups;
    result->group_ids = XCALLOC(int, num_groups);
    result->items = XCALLOC(nccmp_var_stats_array_t, num_groups);

    return result;
}

void nccmp_destroy_group_stats_array(nccmp_group_stats_array_t *array)
{
    int i;
    if (array) {
        for(i = 0; i < array->num_items; ++i) {
            XFREE(array->items[i].items);
        }
        XFREE(array->group_ids);
        XFREE(array->items);
        XFREE(array);
    }
}

void nccmp_init_group_stats(nccmp_group_stats_array_t *group_array, int groupid, int num_vars)
{
    int i, j;
    nccmp_var_stats_array_t *stats_array = NULL;
    for(i = 0; i < group_array->num_items; ++i) {
        if (groupid == group_array->group_ids[i]) {
            stats_array = & (group_array->items[i]);
            stats_array->num_items = num_vars;
            stats_array->items = XCALLOC(nccmp_var_stats_t, num_vars);
            nccmp_clear_var_stats_array(stats_array);
            for(j = 0; j < num_vars; ++j) {
                LOG_DEBUG("FIXME groupid=%d varid=%d count=%d min=%g max=%g sum=%g absum=%g sumsq=%g\n",
                          groupid, j, stats_array->items[j].count, stats_array->items[j].min,
                          stats_array->items[j].max, stats_array->items[j].sum, stats_array->items[j].abs_sum,
                          stats_array->items[j].sum_squares);
            }
            return;
        }
    }
    LOG_ERROR("Did not find in stats array groupid=%d\n", groupid);
}

nccmp_var_stats_array_t* nccmp_find_group_var_stats_array(nccmp_group_stats_array_t *array, int group_id)
{
    int i;

    if (!array) {
        return NULL;
    }

    for(i = 0; i < array->num_items; ++i) {
        if (group_id == array->group_ids[i]) {
            return & array->items[i];
        }
    }

    return NULL;
}
