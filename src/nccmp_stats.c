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
    if (data && (data->count > 1) ) {
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

        if (array[i].name) {
            memset(array[i].name, 0, strlen(array[i].name));
        }
        if (array[i].group_name) {
            memset(array[i].group_name, 0, strlen(array[i].group_name));
        }
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
    result->num_stats = XCALLOC(int*, num_groups);
    result->stats = XCALLOC(nccmp_stats_t**, num_groups);
    return result;
}

void nccmp_destroy_stats_array(nccmp_stats_t *stats, int n)
{
    int i;
    LOG_DEBUG("FIXME nccmp_destroy_stats_array stats=%p n=%d\n", (void*)stats, n);
    if (stats) {
    	LOG_DEBUG("FIXME stats=%p n=%d\n", stats, n);
        for(i = 0; i < n; ++i) {
        	LOG_DEBUG("FIXME i=%d name=%s group_name=%s\n", i, stats[i].name, stats[i].group_name);
            XFREE(stats[i].name);
            XFREE(stats[i].group_name);
            #if HAVE_PTHREAD
                pthread_mutex_destroy(& stats[i].lock);
            #endif

        }
        XFREE(stats);
    }
}

void nccmp_destroy_stats_arrays(nccmp_stats_t **var_stats, int num_vars, int *num_stats)
{
    int var_i;

    LOG_DEBUG("FIXME nccmp_destroy_stats_arrays var_stats=%p num_vars=%d num_stats=%p\n",
    		  (void*)var_stats, num_vars, (void*)num_stats);
    if (var_stats) {
        for(var_i = 0; var_i < num_vars; ++var_i) {
            nccmp_destroy_stats_array(var_stats[var_i], num_stats[var_i]);
        }

        XFREE(var_stats);
    }
    if (num_stats) {
    	XFREE(num_stats);
    }
}

void nccmp_destroy_group_stats(nccmp_group_stats_t *group_stats)
{
    int group_i;
    LOG_DEBUG("FIXME nccmp_destroy_group_stats\n");
    if (group_stats) {
        for(group_i = 0; group_i < group_stats->num_groups; ++group_i) {
        	nccmp_destroy_stats_arrays(group_stats->stats[group_i],
        			                   group_stats->num_vars[group_i],
									   group_stats->num_stats[group_i]);
        }
        XFREE(group_stats->group_ids);
        XFREE(group_stats->num_vars);
        XFREE(group_stats->num_stats);
        XFREE(group_stats->stats);
        XFREE(group_stats);
    }
}

void nccmp_init_group_stats(
        nccmp_group_stats_t *group_stats,
        int groupid, const char *group_name,
        nccmp_var_t *vars, int num_vars,
        nccmp_field_hashmap_t *field_hashmap)
{
    int group_i, num_stats, var_i;
    int FIXME = 0;

    if (!group_stats) {
        return;
    }

    if (FIXME == 0) {
        return;
    }

    for(group_i = 0; group_i < group_stats->num_groups; ++group_i) {
        if (groupid == group_stats->group_ids[group_i]) {
            group_stats->num_vars[group_i] = num_vars;
            group_stats->num_stats[group_i] = XMALLOC(int, num_vars);

            /* If a var has no fields (is not compound), then create at least one stat. */
            num_stats = nccmp_max_int(field_hashmap->num_items, 1);
            memset(group_stats->num_stats[group_i], num_stats, num_vars);

            group_stats->stats[group_i] = XCALLOC(nccmp_stats_t*, num_vars);

            for(var_i = 0; var_i < num_vars; ++var_i) {
                group_stats->stats[group_i][var_i] = XCALLOC(nccmp_stats_t, num_stats);
                nccmp_clear_var_stats_array(group_stats->stats[group_i][var_i], num_stats);
                nccmp_set_var_stat_names(
                        group_stats->stats[group_i][var_i], num_stats,
                        & vars[var_i], field_hashmap, group_name);
            }

            return;
        }
    }
    LOG_ERROR("Did not find in stats array groupid=%d\n", groupid);
}

void nccmp_set_var_stat_names(nccmp_stats_t *var_stats_array, int num_stats,
        nccmp_var_t *var, nccmp_field_hashmap_t *field_hashmap,
        const char* group_name)
{
    int stat_i;

#define ALLOC_AND_COPY(DST, SRC) {              \
        DST = XCALLOC(char, strlen(SRC) + 1);   \
        strcpy(DST, SRC);                       \
    }

    if (nccmp_is_user_type_id(var->type)) {
        for(stat_i = 0; stat_i < num_stats; ++stat_i) {
            nccmp_replace_delimited_prefix(
                    field_hashmap->items[stat_i]->tree_name,
                    var->name,
                    '.',
                    & var_stats_array[stat_i].name);
            ALLOC_AND_COPY(var_stats_array[stat_i].group_name, group_name);
        }
    } else{
        ALLOC_AND_COPY(var_stats_array[0].name, var->name);
        ALLOC_AND_COPY(var_stats_array[0].group_name, group_name);
    }

#undef ALLOC_AND_COPY
}

nccmp_stats_t** nccmp_find_group_var_stats(nccmp_group_stats_t *group_stats, int group_id)
{
    int i;

    if (!group_stats) {
        return NULL;
    }

    for(i = 0; i < group_stats->num_groups; ++i) {
        if (group_id == group_stats->group_ids[i]) {
            return group_stats->stats[i];
        }
    }

    return NULL;
}

void nccmp_print_var_stat_pointers(nccmp_stats_t *stats, int groupid, int num_vars)
{
    int j = 0;
    for(j = 0; j < num_vars; ++j) {
        LOG_DEBUG("nccmp_print_var_stat_pointers groupid=%d varid=%d stats=%p stat=%p count=%d min=%g max=%g sum=%g absum=%g sumsq=%g\n",
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
