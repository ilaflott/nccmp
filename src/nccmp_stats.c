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
    LOG_DEBUG("FIXME nccmp_clear_var_stats_array\n");

    if (!array) {
        return;
    }

    for(i = 0; i < n; ++i) {
        LOG_DEBUG("FIXME   i=%d\n", i);
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
        int group_id, const char *group_name,
        nccmp_var_t *vars, int num_vars,
        nccmp_user_type_hashmap_t *field_hashmap)
{
    int group_i, num_stats, var_i;
    int FIXME_return = 0;

    LOG_DEBUG("FIXME nccmp_init_group_stats\n");

    if (!group_stats) {
        return;
    }

    if (FIXME_return) {
        return;
    }

    for(group_i = 0; group_i < group_stats->num_groups; ++group_i) {
    	LOG_DEBUG("FIXME   group_i=%d group_id=%d group_ids[]=%d\n",
    			  group_i, group_id, group_stats->group_ids[group_i]);
        if (group_id == group_stats->group_ids[group_i]) {
            group_stats->num_vars[group_i] = num_vars;
            group_stats->num_stats[group_i] = XMALLOC(int, num_vars);

            /* If a var has no fields (is not compound), then create at least one stat. */
            num_stats = nccmp_max_int(field_hashmap->num_items, 1);
            LOG_DEBUG("FIXME   num_stats=%d num_vars=%d\n", num_stats, num_vars);
            memset(group_stats->num_stats[group_i], num_stats, num_vars);

            group_stats->stats[group_i] = XCALLOC(nccmp_stats_t*, num_vars);

            for(var_i = 0; var_i < num_vars; ++var_i) {
            	LOG_DEBUG("FIXME     var_i=%d\n", var_i);
                group_stats->stats[group_i][var_i] = XCALLOC(nccmp_stats_t, num_stats);
                nccmp_clear_var_stats_array(group_stats->stats[group_i][var_i], num_stats);
                nccmp_set_var_stat_names(
                        group_stats->stats[group_i][var_i], num_stats,
                        & vars[var_i], field_hashmap, group_name);
            }

            return;
        }
    }
    LOG_ERROR("Did not find in stats array group_id=%d\n", group_id);
}

void nccmp_set_var_stat_names(nccmp_stats_t *var_stats_array, int num_stats,
        nccmp_var_t *var, nccmp_user_type_hashmap_t *fields,
        const char* group_name)
{
    int stat_i;

    LOG_DEBUG("FIXME nccmp_set_var_stat_names\n");

#define ALLOC_AND_COPY(DST, SRC) {              \
        DST = XCALLOC(char, strlen(SRC) + 1);   \
        strcpy(DST, SRC);                       \
    }

	LOG_DEBUG("FIXME   var_name=%s var.type=%d group_name=%s\n", var->name, var->type, group_name);
    LOG_DEBUG("FIXME fields.num_items=%d fields=%p\n", fields ? fields->num_items : -1, (void*)fields);

    if (nccmp_is_user_type_id(var->type)) {
        for(stat_i = 0; stat_i < num_stats; ++stat_i) {
        	LOG_DEBUG("FIXME   stat_i=%d\n", stat_i);
        	LOG_DEBUG("FIXME   tree_name=%s\n", fields->items[stat_i]->tree_name);
            nccmp_replace_delimited_prefix(
                    fields->items[stat_i]->tree_name,
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

void nccmp_print_group_stats_debug(nccmp_group_stats_t *group_stats)
{
    int group_i, var_i, stat_i;
    nccmp_stats_t *stat;

    LOG_DEBUG("FIXME nccmp_print_group_stats_debug\n");

    if (!group_stats) {
    	return;
    }

    for(group_i = 0; group_i < group_stats->num_groups; ++group_i) {
    	LOG_DEBUG("FIXME group_i=%d num_vars=%d\n", group_i, group_stats->num_vars[group_i]);
    	for(var_i = 0; var_i < group_stats->num_vars[group_i]; ++var_i) {
    		LOG_DEBUG("FIXME  var_i=%d num_stats=%d stats=%p\n", var_i, group_stats->num_stats[group_i][var_i], group_stats->stats[group_i][var_i]);
    		for(stat_i = 0; stat_i < group_stats->num_stats[group_i][var_i]; ++stat_i) {
    			stat = & group_stats->stats[group_i][var_i][stat_i];
    	        LOG_DEBUG("FIXME    stat_i=%d stat=%p count=%d name=%s group_name=%s "
    	        		  "min=%g max=%g sum=%g absum=%g sumsq=%g\n",
                          stat_i, (void*)stat, stat->count, stat->name, stat->group_name,
				          stat->min, stat->max, stat->sum, stat->abs_sum, stat->sum_squares);
    		}
    	}
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
