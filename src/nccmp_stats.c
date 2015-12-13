#include <float.h>
#include <log.h>
#include <math.h>
#include <nccmp_stats.h>
#include <nccmp_common.h>
#include <nccmp_utils.h>

double nccmp_compute_var_stats_mean(nccmp_stats_t *data)
{
    if (data && (data->count > 0) ) {
        return data->sum / data->count;
    } else {
        return 0.0;
    }
}

double nccmp_compute_var_stats_stdev(nccmp_stats_t *data)
{
    if (data && (data->count > 0) ) {
        return sqrt( (data->sum_squares - (data->sum * data->sum)/data->count) /
                     (data->count - 1) );
    } else {
        return 0.0;
    }
}

void nccmp_clear_var_stats_array(nccmp_stats_t *array, int n)
{
    int i;
    if (!array) {
        return;
    }

    for(i = 0; i < n; ++i) {
        array[i].count = 0;
        array[i].min = INFINITY;
        array[i].max = -INFINITY;
        array[i].sum = 0.0;
        array[i].abs_sum = 0.0;
        array[i].sum_squares = 0.0;

        #if HAVE_PTHREAD
            pthread_mutex_init(& array[i].lock, NULL);
        #endif
    }
}

nccmp_group_stats_t* nccmp_create_group_stats_array(int num_groups)
{
    nccmp_group_stats_t *result = XMALLOC(nccmp_group_stats_t, 1);
    result->num_groups = num_groups;
    result->group_ids = XCALLOC(int, num_groups);
    result->num_vars = XCALLOC(int, num_groups);
    result->num_fields = XCALLOC(int, num_groups);
    result->var_stats = XCALLOC(nccmp_stats_t*, num_groups);
    result->field_stats = XCALLOC(nccmp_stats_t*, num_groups);

    return result;
}

void nccmp_destroy_group_stats(nccmp_group_stats_t *group_stats)
{
    int i, j;
    if (group_stats) {
        for(i = 0; i < group_stats->num_groups; ++i) {
            #if HAVE_PTHREAD
                for(j = 0; j < group_stats->num_vars[i]; ++j) {
                    pthread_mutex_destroy(& group_stats->var_stats[i][j].lock);
                }
                for(j = 0; j < group_stats->num_fields[i]; ++j) {
                    pthread_mutex_destroy(& group_stats->field_stats[i][j].lock);
                }
            #endif
            XFREE(group_stats->var_stats[i]);
            XFREE(group_stats->field_stats[i]);
        }
        XFREE(group_stats->var_stats);
        XFREE(group_stats->field_stats);
        XFREE(group_stats->group_ids);
        XFREE(group_stats->num_vars);
        XFREE(group_stats->num_fields);
        XFREE(group_stats);
    }
}

void nccmp_init_group_stats(nccmp_group_stats_t *group_stats, int groupid,
        int num_vars, int num_fields)
{
    int i;
    for(i = 0; i < group_stats->num_groups; ++i) {
        if (groupid == group_stats->group_ids[i]) {
            group_stats->var_stats[i] = XCALLOC(nccmp_stats_t, num_vars);
            group_stats->field_stats[i] = XCALLOC(nccmp_stats_t, num_fields);
            group_stats->num_vars[i] = num_vars;
            group_stats->num_fields[i] = num_fields;
            nccmp_clear_var_stats_array(group_stats->var_stats[i], num_vars);
            nccmp_clear_var_stats_array(group_stats->field_stats[i], num_fields);
            return;
        }
    }
    LOG_ERROR("Did not find in stats array groupid=%d\n", groupid);
}

nccmp_stats_t* nccmp_find_var_stats_array(nccmp_group_stats_t *group_stats, int group_id)
{
    int i;

    if (!group_stats) {
        return NULL;
    }

    for(i = 0; i < group_stats->num_groups; ++i) {
        if (group_id == group_stats->group_ids[i]) {
            return group_stats->var_stats[i];
        }
    }

    return NULL;
}

void nccmp_print_var_stat_pointers(nccmp_stats_t *stats, int groupid, int num_vars)
{
    int j = 0;
    for(j = 0; j < num_vars; ++j) {
        LOG_DEBUG("FIXME nccmp_print_var_stat_pointers groupid=%d varid=%d stats=%p stat=%p count=%d min=%g max=%g sum=%g absum=%g sumsq=%g\n",
                  groupid, j, (void*)stats, (void*)&(stats[j]), stats[j].count, stats[j].min,
                  stats[j].max, stats[j].sum, stats[j].abs_sum,
                  stats[j].sum_squares);
    }
}

void nccmp_update_stats(nccmp_stats_t *stat, double delta)
{
    ++stat->count;
    stat->max = nccmp_max_double(stat->max, delta);
    stat->min = nccmp_min_double(stat->min, delta);
    stat->sum += delta;
    stat->abs_sum += delta < 0 ? -delta : delta;
    stat->sum_squares += delta * delta;
}
